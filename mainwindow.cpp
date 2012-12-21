#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(OpenImage()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(SaveImage()));
    connect(ui->actionInvert, SIGNAL(triggered()), this, SLOT(InvertImage()));
    connect(ui->actionGrayscale, SIGNAL(triggered()), this, SLOT(GrayScaleImage()));
    connect(ui->actionEdge_Detect, SIGNAL(triggered()), this, SLOT(EdgeDetect()));
    connect(ui->blueSlider, SIGNAL(actionTriggered(int)), this, SLOT(IncreaseBlue(int)));
    connect(ui->greenSlider, SIGNAL(actionTriggered(int)), this, SLOT(IncreaseGreen(int)));
    connect(ui->redSlider, SIGNAL(actionTriggered(int)), this, SLOT(IncreaseRed(int)));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(About()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OpenImage()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("Select Image"),"C:\\Users\\SESA201948\\Pictures",tr("Image Files(*.png *.jpg)"));
    std::string filePath = fileName.toLocal8Bit().data();
    image = cv::imread(filePath);
    ShowImage(image);
}

void MainWindow::SaveImage()
{
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Imag"),"C:\\Users\\SESA201948\\Pictures", tr("Image Files(*.jpg)"));
    if(!filePath.isEmpty() && !filePath.isNull())
    {
        cv::imwrite(filePath.toLocal8Bit().data(),image,compression_params);
    }
}

void MainWindow::InvertImage()
{
    int i,j,k;
    int step = image.step;
    int channels = image.channels();
    uchar* data= image.data;
    for(i=0; i < image.rows; i++)
        for(j=0; j < image.cols; j++)
            for(k=0; k < channels; k++)
        {
            data[i*step+j*channels+k]=255-data[i*step+j*channels+k];
        }

    ShowImage(image);
}

void MainWindow::IncreaseRed(int value)
{
    IncreaseChannelValue(2, value);
    ShowImage(image);
}

void MainWindow::IncreaseBlue(int value)
{
    IncreaseChannelValue(0, value);
    ShowImage(image);
}

void MainWindow::IncreaseGreen(int value)
{
    IncreaseChannelValue(1, value);
    ShowImage(image);
}

void MainWindow::IncreaseChannelValue(int channel, int value)
{
    int i,j,k = channel;
    int step = image.step;
    uchar* data = image.data;
    int channels = image.channels();
    if(channels >= 3)
    {
        for(i=0; i < image.rows; i++)
            for(j=0; j < image.cols; j++)
            {
                data[i*step+j*channels+k] = value;
            }
    }
}

void MainWindow::GrayScaleImage()
{
    cv::Mat _tmp;
    if(image.channels() != 1)
    {
        cvtColor(image, _tmp, CV_BGR2GRAY);
        image = _tmp;
    }

    ShowImage(image);
}

void MainWindow::EdgeDetect()
{
    int lowThreshold = 0;
    bool ok;
    lowThreshold = QInputDialog::getInt(this,tr("Enter Threshold"), tr("Theshold: "), 3, 0, 100, 1, &ok);
    if(ok)
    {
        if(image.channels() != 1)
        {
            cvtColor(image, image, CV_BGR2GRAY);
        }
        //blur(image, image, cv::Size(3,3));
        Canny(image, image, 0, lowThreshold*3, 3);
        ShowImage(image);
    }
}

void MainWindow::About()
{
    QMessageBox::about(this, tr("About Shredder"), tr("User OpenCV 2.4 and Qt 4.8.2"));
}

void MainWindow::ShowImage(const cv::Mat& matImage)
{
    cv::Mat _tmp;
    switch(matImage.type())
    {
        case CV_8UC1:
            cvtColor(matImage, _tmp, CV_GRAY2RGB);
            break;
        case CV_8UC3:
            cvtColor(matImage, _tmp, CV_BGR2RGB);
            break;
    }

    assert(_tmp.isContinuous());
    QImage _qimage = QImage(_tmp.data, _tmp.cols, _tmp.rows, _tmp.cols*3, QImage::Format_RGB888);
    Redraw(_qimage);
}

void MainWindow::Redraw(QImage _qimage)
{
    ui->imageLabel->setPixmap(QPixmap::fromImage(_qimage));
}
