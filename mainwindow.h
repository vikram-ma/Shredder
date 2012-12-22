#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <QMainWindow>
#include <QFileDialog>
#include <QImage>
#include <QSlider>
#include <QRect>
#include <QMessageBox>
#include <QInputDialog>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    cv::Mat image;
    QLabel *statusLabel;
    void Redraw(const QImage &_qimage);
    void ShowImage(const cv::Mat& matImage);
    void IncreaseChannelValue(int channel, int value);

public slots:
    void OpenImage();
    void SaveImage();
    void InvertImage();
    void GrayScaleImage();
    void IncreaseRed();
    void IncreaseBlue();
    void IncreaseGreen();
    void EdgeDetect();
    void FaceDetect();
    void EyeDetect();
    void RedEyeRemoval();
    void About();
};

#endif // MAINWINDOW_H
