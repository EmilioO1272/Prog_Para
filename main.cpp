#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <filesystem>

int main()
{
    std::string imag_path;
    std::cout << "Pon la direccion de tu imagen";
    std::cin >> imag_path;

    if(!std::filesystem::exists(imag_path))
    {
        std::cout << "El archivo no existe en la direccion especificada";
        return -1;
    }

    cv::Mat imagen = cv::imread(imag_path);

    if(imagen.empty())
    {
        std::cout << "Error al cargar la imagen" << std::endl;
        return -1;
    }
    else
    {
        std::cout << "La imagen cargo con exito" << std::endl;
    }

    cv::imshow("Image", imagen);

    cv::waitKey(0);

    cv::Mat bgr[3];
    cv::split(imagen, bgr);

    cv::Mat blueChannel, greenChannel, redChannel;
    cv::applyColorMap(bgr[0], blueChannel, cv::COLORMAP_TWILIGHT_SHIFTED);
    cv::applyColorMap(bgr[1], greenChannel, cv::COLORMAP_HSV);
    cv::applyColorMap(bgr[2], redChannel, cv::COLORMAP_BONE);

    cv::imshow("blueChannel", blueChannel);
    cv::imshow("greenChannel", greenChannel);
    cv::imshow("redChannel", redChannel);
    cv::waitKey(0);
}