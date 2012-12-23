#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusLabel = new QLabel();
    ui->statusBar->addWidget(statusLabel);

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(OpenImage()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(SaveImage()));
    connect(ui->actionInvert, SIGNAL(triggered()), this, SLOT(InvertImage()));
    connect(ui->actionGrayscale, SIGNAL(triggered()), this, SLOT(GrayScaleImage()));
    connect(ui->actionEdge_Detect, SIGNAL(triggered()), this, SLOT(EdgeDetect()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(About()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionRed, SIGNAL(triggered()), this, SLOT(IncreaseRed()));
    connect(ui->actionBlue, SIGNAL(triggered()), this, SLOT(IncreaseBlue()));
    connect(ui->actionGreen, SIGNAL(triggered()), this, SLOT(IncreaseGreen()));
    connect(ui->actionFace_Detect, SIGNAL(triggered()), this, SLOT(FaceDetect()));
    connect(ui->actionEye_Detect, SIGNAL(triggered()), this, SLOT(EyeDetect()));
    connect(ui->actionRed_Eye_Removal, SIGNAL(triggered()), this, SLOT(RedEyeRemoval()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OpenImage()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,tr("Select Image"),"C:",tr("Image Files(*.png *.jpg)"));
    std::string filePath = fileName.toLocal8Bit().data();
    image = cv::imread(filePath);
    ShowImage(image);
}

void MainWindow::SaveImage()
{
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Imag"),"C:", tr("Image Files(*.jpg)"));
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

void MainWindow::IncreaseRed()
{
    int value;
    bool ok;
    value = QInputDialog::getInt(this,tr("Enter Threshold"), tr("Theshold: "), 50, 0, 255, 1, &ok);
    if(ok)
    {
        IncreaseChannelValue(2, value);
        ShowImage(image);
    }
}

void MainWindow::IncreaseBlue()
{
    int value;
    bool ok;
    value = QInputDialog::getInt(this,tr("Enter Threshold"), tr("Theshold: "), 50, 0, 255, 1, &ok);
    if(ok)
    {
        IncreaseChannelValue(0, value);
        ShowImage(image);
    }
}

void MainWindow::IncreaseGreen()
{
    int value;
    bool ok;
    value = QInputDialog::getInt(this,tr("Enter Threshold"), tr("Theshold: "), 50, 0, 255, 1, &ok);
    if(ok)
    {
        IncreaseChannelValue(1, value);
        ShowImage(image);
    }
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

        blur(image, image, cv::Size(3,3));
        Canny(image, image, 0, lowThreshold*3, 3);
        ShowImage(image);
    }
}

void MainWindow::FaceDetect()
{
    statusLabel->setText("I have started searching, Just curious to know whom you are searching for? ");
    //-- Face detection!
    cv::Mat gray_image;
    if(image.channels()!=1)
    {
        cvtColor(image, gray_image, CV_BGR2GRAY);
    }
    else
    {
        image.copyTo(gray_image);
    }

    equalizeHist(gray_image, gray_image);

    //-- Detect face
    std::vector<cv::Rect> faces;
    cv::CascadeClassifier face_cascade;
    std::string face_cascade_name = "haarcascade_frontalface_alt.xml";
    if(!face_cascade.load(face_cascade_name))
    {
        QMessageBox::warning(this, tr("Facial"), tr("failed to load .xml file"));
        return;
    }

    face_cascade.detectMultiScale(gray_image, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30,30));

    //-- Draw ellipse around all the faces deteceted.
    for(int i = 0; i < faces.size(); i++)
    {
        cv::Point center(faces[i].x + faces[i].width * 0.5, faces[i].y + faces[i].height * 0.5);
        ellipse(image, center, cv::Size(faces[i].width * 0.5, faces[i].height * 0.5), 0, 0, 360, cv::Scalar( 255, 0, 255), 4, 8, 0);
    }

    ShowImage(image);
    statusLabel->setText("I am done searching! Did you find the one you were looking for? ");
}

void MainWindow::EyeDetect()
{
    statusLabel->setText("Searching!, and thinking :). Look here for more updates. ");
    //-- Face detection!
    cv::Mat gray_image;
    if(image.channels()!=1)
    {
        cvtColor(image, gray_image, CV_BGR2GRAY);
    }
    else
    {
        image.copyTo(gray_image);
    }

    equalizeHist(gray_image, gray_image);

    //-- Detect face
    std::vector<cv::Rect> faces;
    cv::CascadeClassifier face_cascade;
    std::string face_cascade_name = "haarcascade_frontalface_alt.xml";
    if(!face_cascade.load(face_cascade_name))
    {
        QMessageBox::warning(this, tr("Face detection"), tr("failed to load .xml file. "));
        return;
    }

    cv::CascadeClassifier eyes_cascade;
    std::string eye_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
    if(!eyes_cascade.load(eye_cascade_name))
    {
        QMessageBox::warning(this, tr("Eye detection"), tr("Failed to data file. "));
        return;
    }

    face_cascade.detectMultiScale(gray_image, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30,30));

    //-- Draw ellipse around all the faces deteceted.
    for(int i = 0; i < faces.size(); i++)
    {
        //cv::Point center(faces[i].x + faces[i].width * 0.5, faces[i].y + faces[i].height * 0.5);
        //ellipse(image, center, cv::Size(faces[i].width * 0.5, faces[i].height * 0.5), 0, 0, 360, cv::Scalar( 255, 0, 255), 4, 8, 0);

        cv::Mat faceROI = gray_image(faces[i]);
        std::vector<cv::Rect> eyes;

        //-- In each face detect eyes.
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
        for(int j=0; j < eyes.size(); j++)
        {
            cv::Point center(faces[i].x + eyes[j].x + eyes[j].width * 0.5, faces[i].y + eyes[j].y + eyes[j].height * 0.5);
            int radius = cvRound( (eyes[j].width + eyes[j].height) * 0.5);
            //circle(image, center, radius, cv::Scalar(255,0,0), 4, 8, 0);
            cv::Point pt1;
            pt1.x = faces[i].x + eyes[j].x;
            pt1.y = faces[i].y + eyes[j].y;
            cv::Point pt2;
            pt2.x = faces[i].x + eyes[j].x + eyes[j].width;
            pt2.y = faces[i].y + eyes[j].y + eyes[j].height;
            rectangle(image, pt1, pt2, cv::Scalar(255,0,0));
        }
    }

    ShowImage(image);
    statusLabel->setText("I am done! ");
}

void MainWindow::RedEyeRemoval()
{
    if(image.channels() == 1)
    {
        QMessageBox::warning(this, tr("Can't remove red eye"), tr("Can't remove the red eye from single channel image"));
        return;
    }

    statusLabel->setText("Ever wondered what causes Red eye effect? One has to understand problem before fixing it!");

    //-- Face detection!
    cv::Mat gray_image;
    if(image.channels()!=1)
    {
        cvtColor(image, gray_image, CV_BGR2GRAY);
    }
    else
    {
        image.copyTo(gray_image);
    }

    equalizeHist(gray_image, gray_image);

    //-- Detect face
    std::vector<cv::Rect> faces;
    cv::CascadeClassifier face_cascade;
    std::string face_cascade_name = "haarcascade_frontalface_alt.xml";
    if(!face_cascade.load(face_cascade_name))
    {
        QMessageBox::warning(this, tr("Face detection"), tr("failed to load .xml file. "));
        return;
    }

    cv::CascadeClassifier eyes_cascade;
    //std::string eye_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
    std::string eye_cascade_name = "haarcascade_eye.xml"; // this looks to be better than tree eyeglasses.
    if(!eyes_cascade.load(eye_cascade_name))
    {
        QMessageBox::warning(this, tr("Eye detection"), tr("Failed to data file. "));
        return;
    }

    face_cascade.detectMultiScale(gray_image, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30,30));

    //-- Search for the eyes in faces detected.
    for(int i = 0; i < faces.size(); i++)
    {
        cv::Mat faceROI = gray_image(faces[i]);
        std::vector<cv::Rect> eyes;

        //-- In each face detect eyes.
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
        for(int j=0; j < eyes.size(); j++)
        {
            //-- Search for red pixels and fix red eye.

            cv::Rect eyeRect;
            eyeRect.height = eyes[j].height;
            eyeRect.width = eyes[j].width;
            eyeRect.x = eyes[j].x + faces[i].x;
            eyeRect.y = eyes[j].y + faces[i].y;

            cv::Mat eyeRoI = image(eyeRect);

            cv::Mat grayEyeROI = gray_image(eyeRect);
            std::vector<cv::Vec3f> circles;

            //-- From this eyeROI find the circular region, then remove red pixels.
            /*HoughCircles(grayEyeROI, circles, CV_HOUGH_GRADIENT, 1, grayEyeROI.rows/8, 200, 100);
            for(size_t p = 0; p < circles.size(); p++)
            {
                // center is circles[p][0] crcles[p][1]
                // radius is circles[p][2]
                cv::Rect fromCircle;
                fromCircle.x = circles[p][0] - (float)(circles[p][2]/2);
                fromCircle.y = circles[p][1] - (float)(circles[p][2]/2);
                fromCircle.height = fromCircle.width = circles[p][2];
                cv::Mat pupilROI = image(fromCircle);
                int channels = pupilROI.channels();
                int r = 2, b = 0, g = 1;
                uchar* data = pupilROI.data;
                int step = pupilROI.step;
                for(int i=0; i<pupilROI.rows; i++)
                {
                    for(int j=0; j < pupilROI.cols; j++)
                    {
                        float redIntensity = (float)data[i*step+j*channels+r]/ ((data[i*step+j*channels+g] + data[i*step+j*channels+b])/ 2);
                        if(redIntensity > 1.5)
                        {
                            data[i*step+j*channels+r] = (data[i*step+j*channels+g] + data[i*step+j*channels+b] ) /2;
                        }
                    }
                }
            }*/

            //-- In this matrix search for red pixels.
            int channels = eyeRoI.channels();
            int r = 2; // data is stored as BGR, so 2 for red.
            int b = 0, g = 1;
            uchar* data = eyeRoI.data;
            int step = eyeRoI.step;
            for(int i=0; i < eyeRoI.rows; i++)
            {
                for(int j=0; j < eyeRoI.cols; j++)
                {
                    float redIntensity = (float)data[i*step+j*channels+r]/ ((data[i*step+j*channels+g] + data[i*step+j*channels+b])/ 2);
                    if(redIntensity > 1.5)
                    {
                        data[i*step+j*channels+r] = (data[i*step+j*channels+g] + data[i*step+j*channels+b] ) /2;
                    }
                }
            }
        }
    }

    ShowImage(image);
    statusLabel->setText("But I didn't have to understand what caused it to fix it :P ");
}

void MainWindow::About()
{
    QMessageBox::about(this, tr("About Shredder"), tr("<p>The <b>Shredder</b> uses OpenCV 2.4 and Qt 4.8.2 "
                                                      "<br/>List of features avialable include "
                                                      "Inverting Image, Edge detection, Change RGB values, "
                                                      "Face detection<p>"));
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

void MainWindow::Redraw(const QImage& _qimage)
{
    ui->imageLabel->setPixmap(QPixmap::fromImage(_qimage));
}
