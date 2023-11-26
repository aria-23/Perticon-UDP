# Variables
QT_PATH=/home/perticon/Qt/5.15.2/gcc_64/
IFW_PATH=/home/perticon/Qt/Tools/QtInstallerFramework/4.5/


if ! [ -x "$(command -v cqtdeployer)" ]; then
    echo "Error: cqtdeployer could not be found"
    echo "Please download from https://github.com/QuasarApp/CQtDeployer/releases"
    exit
fi

if [ -d build ]; then
    rm -rf build
fi
if [ -d output ]; then
    rm -rf output
fi

# Create paths
mkdir -p build/cache
rm build/installer/packages/Perticon/data/ -r
cp installer/linux build/installer -r

cd build/cache
# make -DCMAKE_PREFIX_PATH=$QT_PATH ..
$QT_PATH/bin/qmake ../..
make -j 12
cd ../..


# mkdir -p installer/linux/packages/Perticon/data
cqtdeployer -bin build/cache/Perticon  -qmake $QT_PATH/bin/qmake -verbose 3 -targetDir build/installer/packages/Perticon/data/ -qmlDir .
cp Icons/icon.png build/installer/packages/Perticon/data/logo.png
cp installer/Perticon.sh build/installer/packages/Perticon/data/Perticon.sh

mkdir -p output
$IFW_PATH/bin/binarycreator -c build/installer/config/config.xml -p build/installer/packages/ output/linux.run
