#include <random>
#include <math.h>
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
        const std::size_t X = input.GetSizeX();
        const std::size_t Y = input.GetSizeY();
 
        for (std::size_t x = 0; x < X; ++x) {
            for (std::size_t y = 0; y < Y; ++y) {
                
                float red = 0, gre = 0, blu = 0;

                if (x > 0) {
                    if (y > 0) {
                        red += glm::vec3(input.At(x - 1, y - 1)).r;
                        gre += glm::vec3(input.At(x - 1, y - 1)).g;
                        blu += glm::vec3(input.At(x - 1, y - 1)).b;
                    }
                    red += glm::vec3(input.At(x - 1, y)).r;
                    gre += glm::vec3(input.At(x - 1, y)).g;
                    blu += glm::vec3(input.At(x - 1, y)).b;
                    if (y < Y - 1) {
                        red += glm::vec3(input.At(x - 1, y + 1)).r;
                        gre += glm::vec3(input.At(x - 1, y + 1)).g;
                        blu += glm::vec3(input.At(x - 1, y + 1)).b;
                    }
                }
                
                if (y > 0) {
                    red += glm::vec3(input.At(x, y - 1)).r;
                    gre += glm::vec3(input.At(x, y - 1)).g;
                    blu += glm::vec3(input.At(x, y - 1)).b;
                }
                red += glm::vec3(input.At(x, y)).r;
                gre += glm::vec3(input.At(x, y)).g;
                blu += glm::vec3(input.At(x, y)).b;
                if (y < Y - 1) {
                    red += glm::vec3(input.At(x, y + 1)).r;
                    gre += glm::vec3(input.At(x, y + 1)).g;
                    blu += glm::vec3(input.At(x, y + 1)).b;
                }
                
                if (x < X - 1) {
                    if (y > 0) {
                        red += glm::vec3(input.At(x + 1, y - 1)).r;
                        gre += glm::vec3(input.At(x + 1, y - 1)).g;
                        blu += glm::vec3(input.At(x + 1, y - 1)).b;
                    }
                    red += glm::vec3(input.At(x + 1, y)).r;
                    gre += glm::vec3(input.At(x + 1, y)).g;
                    blu += glm::vec3(input.At(x + 1, y)).b;
                    if (y < Y - 1) {
                        red += glm::vec3(input.At(x + 1, y + 1)).r;
                        gre += glm::vec3(input.At(x + 1, y + 1)).g;
                        blu += glm::vec3(input.At(x + 1, y + 1)).b;
                    }
                }

                output.At(x, y) = {
                    red / 9.0f,
                    gre / 9.0f,
                    blu / 9.0f,
                };
            }
        }

    }

    //write a func to check invalid pos, or it drive me crazy
    bool check(int const& X, int const& Y, int x, int y) {
        return (x > 0 && y > 0 && x < X - 1 && y < Y - 1);
    }


    void Edge(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:
        const int sobelX[3][3] = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
        };

        const int sobelY[3][3] = {
            { 1,  2,  1},
            { 0,  0,  0},
            {-1, -2, -1}
        };

        const int X  = input.GetSizeX();
        const int Y = input.GetSizeY();

        for (std::size_t x = 0; x < X; ++x) {
            for (std::size_t y = 0; y < Y; ++y) {
                float rX = 0.0f, rY = 0.0f;
                float gX = 0.0f, gY = 0.0f;
                float bX = 0.0f, bY = 0.0f;

                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        if (! check(X, Y, x + i, y + j))
                            continue;
                        glm::vec3 color = input.At(x + i, y + j);
                        
                        rX += color.r * sobelX[i + 1][j + 1];
                        rY += color.r * sobelY[i + 1][j + 1];
                        gX += color.g * sobelX[i + 1][j + 1];
                        gY += color.g * sobelY[i + 1][j + 1];
                        bX += color.b * sobelX[i + 1][j + 1];
                        bY += color.b * sobelY[i + 1][j + 1];
                    }
                }

                float r_magnitude = std::sqrt(rX * rX + rY * rY);
                float g_magnitude = std::sqrt(gX * gX + gY * gY);
                float b_magnitude = std::sqrt(bX * bX + bY * bY);

                if (r_magnitude + g_magnitude + b_magnitude > 1.5)
                    output.At(x, y) = {
                        r_magnitude,
                        g_magnitude,
                        b_magnitude
                    };
                else
                    output.At(x, y) = { 0, 0, 0 };
            }
        }

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

        int x0 = p0.x, y0 = p0.y;
        int x1 = p1.x, y1 = p1.y;

        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);

        int step_x = (x0 < x1) ? 1 : -1;
        int step_y = (y0 < y1) ? 1 : -1;

        if (x0 == x1) {
            while (y0 != y1) {
                canvas.At(x0, y0) = color;
                y0 += step_y;
            }
        }
        else if (y0 == y1) {
            while (x0 != x1) {
                canvas.At(x0, y0) = color;
                x0 += step_x;
            }
        }
        else if (dx == dy) {
            while (x0 != x1) {
                canvas.At(x0, y0) = color;
                x0 += step_x;
                y0 += step_y;
            }
        }
        else if (dx > dy) {
            int F = dx / 2;

            while (x0 != x1) {

                canvas.At(x0, y0) = color;

                F -= dy;
                if (F < 0) {
                    y0 += step_y;
                    F += dx;
                }
                x0 += step_x;
            }
        }
        else {
            int F = dy / 2;

            while (y0 != y1) {

                canvas.At(x0, y0) = color;

                F -= dx;
                if (F < 0) {
                    x0 += step_x;
                    F += dy;
                }
                y0 += step_y;
            }
        }

        canvas.At(x1, y1) = color;
    }

    /******************* 5. Triangle Drawing *****************/
    void DrawTriangleFilled(
        ImageRGB &       canvas,
        glm::vec3 const  color,
        glm::ivec2 const p0,
        glm::ivec2 const p1,
        glm::ivec2 const p2) {
        // your code here:
        DrawLine(canvas, color, p0, p1);
        DrawLine(canvas, color, p1, p2);
        DrawLine(canvas, color, p2, p0);
    }

    /******************* 6. Image Supersampling *****************/
    void Supersample(
        ImageRGB &       output,
        ImageRGB const & input,
        int              rate) {
        // your code here:
        const int X = input.GetSizeX();
        const int Y = input.GetSizeY();

        const int width = X / rate;
        const int height = Y / rate;

        output = ImageRGB(width, height);

        int R = rate * rate;

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                float red = 0, gre = 0, blu = 0;

                int pos_x = x * rate;
                int pos_y = y * rate;

                for (int i = 0; i < rate; i++) {
                    for (int j = 0; j < rate; j++) {
                        red += glm::vec3(input.At(pos_x + i, pos_y + j)).r;
                        gre += glm::vec3(input.At(pos_x + i, pos_y + j)).g;
                        blu += glm::vec3(input.At(pos_x + i, pos_y + j)).b;
                    }
                }

                output.At(x, y) = {
                    red / R,
                    gre / R,
                    blu / R,
                };
            }
        }
    }

    /******************* 7. Bezier Curve *****************/
    // Note: Please finish the function [DrawLine] before trying this part.
    glm::vec2 CalculateBezierPoint(
        std::span<glm::vec2> points,
        float const          t) {
        // your code here:
        int num = points.size();

        if (num == 1) {
            return points[0];
        }

        std::vector<glm::vec2> new_points(num - 1);
        float const s = 1 - t;

        for (int i = 0; i < num - 1; i++) {
            new_points[i] = s * points[i] + t * points[i + 1];
        }

        return CalculateBezierPoint(std::span<glm::vec2>(new_points), t);
    }
} // namespace VCX::Labs::Drawing2D