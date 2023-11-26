#include "detector_torchscript_yolo.h"
#include "qdebug.h"

Detector_TorchScript_Yolo::Detector_TorchScript_Yolo(QObject *parent) : Detector{parent} {}

void Detector_TorchScript_Yolo::loadModel(const QString modelPath)
{
    qDebug() << "---" << Q_FUNC_INFO << modelPath;

     if (torch::cuda::is_available()) {
         qDebug() << "------ We are using torch on GPU";
         device_type = torch::kCUDA;
     } else {
         qDebug() << "------ We are using torch on CPU";
         device_type = torch::kCPU;
     }

     try {
         module = torch::jit::load(modelPath.toStdString());
         module.to(device_type);

         modelIsLoaded = true;
         qDebug() << "------ Model loaded:" << modelPath;
     } catch (const c10::Error& e) {
         modelIsLoaded = false;
         qDebug() << "------ Error in loading the model!";
         std::cout<<e.what()<<std::endl;
     }
     qDebug() << "---" << Q_FUNC_INFO << modelPath;
}

void Detector_TorchScript_Yolo::setOutputSize(const cv::Size &newOutputSize)
{
    qDebug() << "---" << Q_FUNC_INFO << newOutputSize.height << newOutputSize.height;
    outputSize = newOutputSize;
}

std::vector<std::vector<Detection> > Detector_TorchScript_Yolo::run(std::vector<cv::Mat> &images)
{
    int batchSize = images.size();
    try {

        auto t0 = std::chrono::high_resolution_clock::now();
        std::vector<cv::Size> orgImagesSize;
        std::vector<PadIno> pads_info = LetterboxImage(images, orgImagesSize);
        torch::Tensor tensor_batch = convertToTensor(images);

        std::vector<torch::jit::IValue> iValue;
        iValue.emplace_back(tensor_batch);
        torch::jit::IValue output = module.forward(iValue); /// QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread


        auto detections = output.toTuple()->elements()[0].toTensor();
        auto result = PostProcessing(detections, pads_info, orgImagesSize);

        auto lap = std::chrono::high_resolution_clock::now();
        auto dt= std::chrono::duration_cast<std::chrono::milliseconds>(lap-t0).count();
        qDebug () <<" ------- Process time: " << dt ;


        return result;

    } catch (const c10::Error& e) {
        qDebug() << "------ Error in inference!";
        std::cout<<e.what()<<std::endl;
        std::vector<std::vector<Detection>> output(batchSize);
        return output;
    }
    return{};
}
std::vector<PadIno> Detector_TorchScript_Yolo::LetterboxImage(std::vector<cv::Mat> &images, std::vector<cv::Size> &orgImagesSize)
{
    std::vector<PadIno> pads_info;
    for (int i = 0; i < static_cast<int>(images.size()); ++i){
        orgImagesSize.push_back(images[i].size());
        PadIno _pad;
        _pad.scale = std::min((float)outputSize.width / images[i].cols, (float)outputSize.height / images[i].rows);
        int mid_h = static_cast<int>(images[i].rows * _pad.scale);
        int mid_w = static_cast<int>(images[i].cols * _pad.scale);

//        cv::resize(images[i], images[i], cv::Size(mid_w, mid_h));
        _pad.top = (static_cast<int>(outputSize.height) - mid_h) / 2;
        _pad.down = (static_cast<int>(outputSize.height)- mid_h + 1) / 2;
        _pad.left = (static_cast<int>(outputSize.width)- mid_w) / 2;
        _pad.right = (static_cast<int>(outputSize.width)- mid_w + 1) / 2;
//        qDebug() << "_pad.top" << _pad.top << "_pad.down" << _pad.down << "_pad.left" << _pad.left << "_pad.right" << _pad.right ;

        cv::copyMakeBorder(images[i], images[i], _pad.top, _pad.down, _pad.left, _pad.right, cv::BORDER_CONSTANT, cv::Scalar(114, 114, 114));
        pads_info.push_back(_pad);
//        qDebug("End LetterboxImage");
    }
    return pads_info;
}
at::Tensor Detector_TorchScript_Yolo::convertToTensor(std::vector<cv::Mat> &images)
{
    for (int i = 0; i < static_cast<int>(images.size()); ++i){
        cv::cvtColor(images[i], images[i], cv::COLOR_BGR2RGB);
        images[i].convertTo(images[i], CV_32FC3, 1.0f / 255.0f); // normalization
    }

    cv::Mat batch_image;
    cv::vconcat(images, batch_image);

    auto batchSize = static_cast<int>(images.size());
    auto rows = outputSize.width;
    auto cols = outputSize.height;
    auto channels = batch_image.channels();

    torch::Tensor input_tensor_batch = torch::from_blob(batch_image.data, {batchSize, rows, cols, channels}).to(device_type);
    input_tensor_batch = input_tensor_batch.permute({0, 3, 1, 2}).contiguous();
//    input_tensor_batch = input_tensor_batch.permute({0, 3, 1, 2});

    return input_tensor_batch;
}
std::vector<std::vector<Detection> > Detector_TorchScript_Yolo::PostProcessing(const at::Tensor &detections, std::vector<PadIno> pads_info, const std::vector<cv::Size> &orgImagesSize)
{
    constexpr int item_attr_size = 5;
    int batch_size = detections.size(0);
    auto num_classes = detections.size(2) - item_attr_size;
    auto conf_mask = detections.select(2, 4).ge(conf_threshold).unsqueeze(2);
//    std::vector<std::vector<Detection>> output(batch_size);
    std::vector<std::vector<Detection>> output;
    output.reserve(batch_size);
    for (int batch_i = 0; batch_i < batch_size; batch_i++) {
        std::vector<Detection> det_vec;
        auto det = torch::masked_select(detections[batch_i], conf_mask[batch_i]).view({-1, num_classes + item_attr_size});
        if (0 == det.size(0)) {
            output.emplace_back(det_vec);
            continue;
        }
        det.slice(1, item_attr_size, item_attr_size + num_classes) *= det.select(1, 4).unsqueeze(1);
        torch::Tensor box = xywh2xyxy(det.slice(1, 0, 4));
        std::tuple<torch::Tensor, torch::Tensor> max_classes = torch::max(det.slice(1, item_attr_size, item_attr_size + num_classes), 1);
        auto max_conf_score = std::get<0>(max_classes);
        auto max_conf_index = std::get<1>(max_classes);

        max_conf_score = max_conf_score.to(torch::kFloat).unsqueeze(1);
        max_conf_index = max_conf_index.to(torch::kFloat).unsqueeze(1);
        det = torch::cat({box.slice(1, 0, 4), max_conf_score, max_conf_index}, 1);
        constexpr int max_wh = 4096;
        auto c = det.slice(1, item_attr_size, item_attr_size + 1) * max_wh;
        auto offset_box = det.slice(1, 0, 4) + c;
        std::vector<cv::Rect> offset_box_vec;
        std::vector<float> score_vec;
        auto offset_boxes_cpu = offset_box.cpu();
        auto det_cpu = det.cpu();
        const auto& det_cpu_array = det_cpu.accessor<float, 2>();
        Tensor2Detection(offset_boxes_cpu.accessor<float,2>(), det_cpu_array, offset_box_vec, score_vec);
        std::vector<int> nms_indices;
        cv::dnn::NMSBoxes(offset_box_vec, score_vec, conf_threshold, iou_threshold, nms_indices);

        for (int index : nms_indices) {
            Detection t;
            const auto& b = det_cpu_array[index];
            t.bbox = cv::Rect(cv::Point(b[Det::tl_x], b[Det::tl_y]), cv::Point(b[Det::br_x], b[Det::br_y]));
            t.confidence = det_cpu_array[index][Det::score];
            t.classID = det_cpu_array[index][Det::class_idx];
            det_vec.emplace_back(t);
        }

        ScaleCoordinates(det_vec, pads_info[batch_i].left, pads_info[batch_i].top, pads_info[batch_i].scale, orgImagesSize[batch_i]);
        output.emplace_back(det_vec);
    }

    return output;
}
at::Tensor Detector_TorchScript_Yolo::xywh2xyxy(const at::Tensor &x)
{
    auto y = torch::zeros_like(x);
    // convert bounding box format from (center x, center y, width, height) to (x1, y1, x2, y2)
    y.select(1, Det::tl_x) = x.select(1, 0) - x.select(1, 2).div(2);
    y.select(1, Det::tl_y) = x.select(1, 1) - x.select(1, 3).div(2);
    y.select(1, Det::br_x) = x.select(1, 0) + x.select(1, 2).div(2);
    y.select(1, Det::br_y) = x.select(1, 1) + x.select(1, 3).div(2);
    return y;
}
void Detector_TorchScript_Yolo::Tensor2Detection(const at::TensorAccessor<float, 2> &offset_boxes, const at::TensorAccessor<float, 2> &det, std::vector<cv::Rect> &offset_box_vec, std::vector<float> &score_vec)
{
    for (int i = 0; i < offset_boxes.size(0) ; i++)
    {
        offset_box_vec.emplace_back(
            cv::Rect(cv::Point(offset_boxes[i][Det::tl_x], offset_boxes[i][Det::tl_y]),
                     cv::Point(offset_boxes[i][Det::br_x], offset_boxes[i][Det::br_y]))
            );
        score_vec.emplace_back(det[i][Det::score]);
    }
}
void Detector_TorchScript_Yolo::ScaleCoordinates(std::vector<Detection> &data, float pad_w, float pad_h, float scale, const cv::Size &img_shape)
{
    auto clip = [](float n, float lower, float upper) {
        return std::max(lower, std::min(n, upper));
    };

    std::vector<Detection> detections;
    for (auto & i : data) {
        float x1 = (i.bbox.tl().x - pad_w)/scale;
        float y1 = (i.bbox.tl().y - pad_h)/scale;
        float x2 = (i.bbox.br().x - pad_w)/scale;
        float y2 = (i.bbox.br().y - pad_h)/scale;

        x1 = clip(x1, 0, img_shape.width);
        y1 = clip(y1, 0, img_shape.height);
        x2 = clip(x2, 0, img_shape.width);
        y2 = clip(y2, 0, img_shape.height);
        i.bbox = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
    }
}
float Detector_TorchScript_Yolo::calcIOU(Detection &trackedBox, Detection &detectedBox)
{
    float dbox_tl_1 = detectedBox.bbox.x;
    float dbox_tl_2 = detectedBox.bbox.y;
    float dbox_br_1 = detectedBox.bbox.x + detectedBox.bbox.width;
    float dbox_br_2 = detectedBox.bbox.y + detectedBox.bbox.height;
    float area_dbbox = detectedBox.bbox.width * detectedBox.bbox.height;

    float tbox_tl_1 = trackedBox.bbox.x;
    float tbox_tl_2 = trackedBox.bbox.y;
    float tbox_br_1 = trackedBox.bbox.x + trackedBox.bbox.width;
    float tbox_br_2 = trackedBox.bbox.y + trackedBox.bbox.height;
    float area_tbbox = trackedBox.bbox.width * trackedBox.bbox.height;

    float tl_1 = std::max(dbox_tl_1, tbox_tl_1);
    float tl_2 = std::max(dbox_tl_2, tbox_tl_2);
    float br_1 = std::min(dbox_br_1, tbox_br_1);
    float br_2 = std::min(dbox_br_2, tbox_br_2);

    float w = br_1 - tl_1;
    w = (w < 0 ? 0 : w);
    float h = br_2 - tl_2;
    h = (h < 0 ? 0 : h);
    float area_intersection = w * h;
    return area_intersection / (area_dbbox + area_tbbox - area_intersection);
}
