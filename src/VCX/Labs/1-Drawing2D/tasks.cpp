#include <random>

#include <spdlog/spdlog.h>

#include "Labs/1-Drawing2D/tasks.h"

using VCX::Labs::Common::ImageRGB;

namespace VCX::Labs::Drawing2D {
    /******************* 1.Image Dithering *****************/
    void DitheringThreshold(
        ImageRGB &       output,
        ImageRGB const & input) {
        for (std::size_t x = 0; x < input.GetSizeX(); ++x)
            for (std::size_t y = 0; y < input.GetSizeY(); ++y) {
                glm::vec3 color = input.At(x, y);
                output.At(x, y) = {
                    color.r > 0.5 ? 1 : 0,
                    color.g > 0.5 ? 1 : 0,
                    color.b > 0.5 ? 1 : 0,
                };
            }
    }

    void DitheringRandomUniform(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<float> distribution(-0.5, 0.5);

        for (std::size_t x = 0; x < input.GetSizeX(); ++x) {
            for (std::size_t y = 0; y < input.GetSizeY(); ++y) {
                glm::vec3 color = input.At(x, y);

                float gray = color.r;
                gray += distribution(generator);

                float binary_value = gray > 0.5 ? 1 : 0;

                output.At(x, y) = {
                    binary_value,
                    binary_value,
                    binary_value,
                };
            }
        }
    }

    void DitheringRandomBlueNoise(
        ImageRGB &       output,
        ImageRGB const & input,
        ImageRGB const & noise) {
        // your code here:

        for (std::size_t x = 0; x < input.GetSizeX(); ++x) {
            for (std::size_t y = 0; y < input.GetSizeY(); ++y) {
                glm::vec3 color       = input.At(x, y);
                glm::vec3 noise_color = noise.At(x, y);

                float gray       = color.r;
                float noise_gray = noise_color.r;

                gray += noise_gray;

                float binary_value = gray > 1 ? 1 : 0;

                output.At(x, y) = {
                    binary_value,
                    binary_value,
                    binary_value,
                };
            }
        }
    }

    void DitheringOrdered(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:

        const int dither_matrix[3][3] = {
            {6, 8, 4},
            {1, 0, 3},
            {5, 2, 7}
        };

        int output_x = input.GetSizeX() * 3;
        int output_y = input.GetSizeY() * 3;

        for (std::size_t x = 0; x < input.GetSizeX(); ++x) {
            for (std::size_t y = 0; y < input.GetSizeY(); ++y) {
                glm::vec3 color       = input.At(x, y);

                float gray       = color.r;

                size_t ox = 3 * x;
                size_t oy = 3 * y;

                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {

                        float output_color = gray > (dither_matrix[i][j] / 9.0f) ? 1 : 0;

                        output.At(ox + i, oy + j) = {
                            output_color,
                            output_color,
                            output_color,
                        };
                    }
                }
            }
        }
    }

    void DitheringErrorDiffuse(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:

        const std::size_t X = input.GetSizeX();
        const std::size_t Y = input.GetSizeY();

        output = input;

        for (std::size_t x = 0; x < X; ++x) {
            for (std::size_t y = 0; y < Y; ++y) {
                glm::vec3 color = output.At(x, y);

                float gray = color.r;
                float delta = 0;
                float new_gray;
                float out_gray = gray > 0.5 ? 1 : 0;

                output.At(x, y) = {
                    out_gray,
                    out_gray,
                    out_gray,
                };

                delta = (gray - out_gray) / 16.0f;

                if (x < X - 1) {
                    new_gray = glm::vec3(output.At(x + 1, y)).r 
                        + 7 * delta;
                    output.At(x + 1, y) = {
                        new_gray,
                        new_gray,
                        new_gray,
                    };
                }
                if (y < Y - 1) {
                    new_gray = glm::vec3(output.At(x, y + 1)).r 
                        + 5 * delta;
                    output.At(x, y + 1) = {
                        new_gray,
                        new_gray,
                        new_gray,
                    };

                    if (x < X - 1) {
                        new_gray = glm::vec3(output.At(x + 1, y + 1)).r 
                            + delta;
                        output.At(x + 1, y + 1) = {
                            new_gray,
                            new_gray,
                            new_gray,
                        };
                    }

                    if (x > 0) {
                        new_gray = glm::vec3(output.At(x - 1, y + 1)).r 
                            + 3 * delta;
                        output.At(x - 1, y + 1) = {
                            new_gray,
                            new_gray,
                            new_gray,
                        };
                    }

                }
            }
        }
    }

    /******************* 2.Image Filtering *****************/
    void Blur(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:
    }

    void Edge(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:
    }

    /******************* 3. Image Inpainting *****************/
    void Inpainting(
        ImageRGB &         output,
        ImageRGB const &   inputBack,
        ImageRGB const &   inputFront,
        const glm::ivec2 & offset) {
        output             = inputBack;
        std::size_t width  = inputFront.GetSizeX();
        std::size_t height = inputFront.GetSizeY();
        glm::vec3 * g      = new glm::vec3[width * height];
        memset(g, 0, sizeof(glm::vec3) * width * height);
        // set boundary condition
        for (std::size_t y = 0; y < height; ++y) {
            // set boundary for (0, y), your code: g[y * width] = ?
            // set boundary for (width - 1, y), your code: g[y * width + width - 1] = ?
        }
        for (std::size_t x = 0; x < width; ++x) {
            // set boundary for (x, 0), your code: g[x] = ?
            // set boundary for (x, height - 1), your code: g[(height - 1) * width + x] = ?
        }

        // Jacobi iteration, solve Ag = b
        for (int iter = 0; iter < 8000; ++iter) {
            for (std::size_t y = 1; y < height - 1; ++y)
                for (std::size_t x = 1; x < width - 1; ++x) {
                    g[y * width + x] = (g[(y - 1) * width + x] + g[(y + 1) * width + x] + g[y * width + x - 1] + g[y * width + x + 1]);
                    g[y * width + x] = g[y * width + x] * glm::vec3(0.25);
                }
        }

        for (std::size_t y = 0; y < inputFront.GetSizeY(); ++y)
            for (std::size_t x = 0; x < inputFront.GetSizeX(); ++x) {
                glm::vec3 color = g[y * width + x] + inputFront.At(x, y);
                output.At(x + offset.x, y + offset.y) = color;
            }
        delete[] g;
    }

    /******************* 4. Line Drawing *****************/
    void DrawLine(
        ImageRGB &       canvas,
        glm::vec3 const  color,
        glm::ivec2 const p0,
        glm::ivec2 const p1) {
        // your code here:
    }

    /******************* 5. Triangle Drawing *****************/
    void DrawTriangleFilled(
        ImageRGB &       canvas,
        glm::vec3 const  color,
        glm::ivec2 const p0,
        glm::ivec2 const p1,
        glm::ivec2 const p2) {
        // your code here:
    }

    /******************* 6. Image Supersampling *****************/
    void Supersample(
        ImageRGB &       output,
        ImageRGB const & input,
        int              rate) {
        // your code here:
    }

    /******************* 7. Bezier Curve *****************/
    // Note: Please finish the function [DrawLine] before trying this part.
    glm::vec2 CalculateBezierPoint(
        std::span<glm::vec2> points,
        float const          t) {
        // your code here:
        return glm::vec2 {0, 0};
    }
} // namespace VCX::Labs::Drawing2D