#include "Labs/5-Visualization/tasks.h"

#include <numbers>

using VCX::Labs::Common::ImageRGB;
namespace VCX::Labs::Visualization {

    struct CoordinateStates {
        // your code here
        std::vector<Car> const & data;
        float                    lines_x[7];
        float                    lines_y[2];
        
        float                    line_width_rect;
        float                    rect_width;
        float                    rect_line_width;
        glm::vec3                ori_color;
        glm::vec3                dest_color;
        Car                      Max;
        Car                      Min;
        // text
        glm::vec4                text_color;
        float                    line_height;
        float                    line_shift_rate;
        float                    shift_rate;
        // data for interactions
        int                      chosen;//chosen 0-6
        glm::vec3                blue;
        glm::vec3                red;
        Car                      inf;
        Car                      sup;
        float                    _inf[7];
        float                    _sup[7];

        CoordinateStates(std::vector<Car> const& c) : data(c){       
            Max = data[0];
            Min = data[0];
            for (auto const& car : data){
                Max.mileage = std::max(Max.mileage, car.mileage);
                Max.cylinders = std::max(Max.cylinders, car.cylinders);
                Max.displacement = std::max(Max.displacement, car.displacement);
                Max.horsepower = std::max(Max.horsepower, car.horsepower);
                Max.weight = std::max(Max.weight, car.weight);
                Max.acceleration = std::max(Max.acceleration, car.acceleration);
                Max.year = std::max(Max.year, car.year);
                Min.mileage = std::min(Min.mileage, car.mileage);
                Min.cylinders = std::min(Min.cylinders, car.cylinders);
                Min.displacement = std::min(Min.displacement, car.displacement);
                Min.horsepower = std::min(Min.horsepower, car.horsepower);
                Min.weight = std::min(Min.weight, car.weight);
                Min.acceleration = std::min(Min.acceleration, car.acceleration);
                Min.year = std::min(Min.year, car.year);
            }
            inf = Min;
            sup = Max;

            lines_y[0] = .2f;
            lines_y[1] = .8f;
            for (int i = 0; i < 7; ++i){
                lines_x[i] = 0.0625f + 0.15f * i;
                _inf[i] = lines_y[1];
                _sup[i] = lines_y[0];
            }

            line_width_rect   = 8.f;
            rect_width        = line_width_rect * 0.002;
            rect_line_width   = .8f;
            ori_color         = glm::vec3(0.2745, 0.5098, 0.70588);
            dest_color        = glm::vec3(0.6470, 0.1647, 0.1647);
            text_color        = glm::vec4(0.f, 0.f, 0.f, 1.f);
            line_height       = 0.018f;
            line_shift_rate   = 2.f;
            shift_rate        = 1.f;
            chosen            = 1;
            blue              = glm::vec3(0, 0, 0.5);
            red               = glm::vec3(0.5, 0, 0);    
        }
        
        void Drawdata(Common::ImageRGB& input,int idx){
            glm::vec3 color; 
            switch (chosen){
            case 0: color = (glm::vec3(data[idx].cylinders - Min.cylinders) * ori_color + glm::vec3(Max.cylinders - data[idx].cylinders) * dest_color) / glm::vec3(Max.cylinders - Min.cylinders);
                break;
            case 1:
                color = (glm::vec3(data[idx].displacement - Min.displacement) * ori_color + glm::vec3(Max.displacement - data[idx].displacement) * dest_color) / glm::vec3(Max.displacement - Min.displacement);
                break;
            case 2:
                color = (glm::vec3(data[idx].weight - Min.weight) * ori_color + glm::vec3(Max.weight - data[idx].weight) * dest_color) / glm::vec3(Max.weight - Min.weight);
                break;
            case 3:
                color = (glm::vec3(data[idx].horsepower - Min.horsepower) * ori_color + glm::vec3(Max.horsepower - data[idx].horsepower) * dest_color) / glm::vec3(Max.horsepower - Min.horsepower);
                break;
            case 4:
                color = (glm::vec3(data[idx].acceleration - Min.acceleration) * ori_color + glm::vec3(Max.acceleration - data[idx].acceleration) * dest_color) / glm::vec3(Max.acceleration - Min.acceleration);
                break;
            case 5:
                color = (glm::vec3(data[idx].mileage - Min.mileage) * ori_color + glm::vec3(Max.mileage - data[idx].mileage) * dest_color) / glm::vec3(Max.mileage - Min.mileage);
                break;
            case 6:
                color = (glm::vec3(data[idx].year - Min.year) * ori_color + glm::vec3(Max.year - data[idx].year) * dest_color) / glm::vec3(Max.year - Min.year);
                break;
            }
            glm::vec4 Color;
            if ((data[idx].year >= inf.year && data[idx].year <= sup.year)
                && (data[idx].cylinders >= inf.cylinders && data[idx].cylinders <= sup.cylinders)
                && (data[idx].displacement >= inf.displacement && data[idx].displacement <= sup.displacement)
                && (data[idx].weight >= inf.weight && data[idx].weight <= sup.weight)
                && (data[idx].horsepower >= inf.horsepower && data[idx].horsepower <= sup.horsepower)
                && (data[idx].acceleration >= inf.acceleration && data[idx].acceleration <= sup.acceleration)
                && (data[idx].mileage >= inf.mileage && data[idx].mileage <= sup.mileage))
                Color = glm::vec4(color, 1.f);
            else
                Color=glm::vec4(blue,1.f);
            float     coord[7] = {
                -(lines_y[1] - lines_y[0]) * (float) (data[idx].cylinders - Min.cylinders) / (Max.cylinders-Min.cylinders) + lines_y[1],
                -(lines_y[1]-lines_y[0]) * (data[idx].displacement-Min.displacement) / (Max.displacement-Min.displacement) + lines_y[1],
                -(lines_y[1]-lines_y[0]) * (data[idx].weight-Min.weight) /( Max.weight-Min.weight) + lines_y[1],
                -(lines_y[1]-lines_y[0]) * (data[idx].horsepower-Min.horsepower) / (Max.horsepower-Min.horsepower) + lines_y[1],
                -(lines_y[1]-lines_y[0]) * (data[idx].acceleration-Min.acceleration) / (Max.acceleration-Min.acceleration) + lines_y[1],
                -(lines_y[1]-lines_y[0]) * (data[idx].mileage-Min.mileage) /( Max.mileage-Min.mileage) + lines_y[1],
                -(lines_y[1]-lines_y[0]) * (float)(data[idx].year-Min.year) /( Max.year-Min.year )+ lines_y[1],
            };
            for (int i = 0; i < 6; ++i){
                DrawLine(input, Color, glm::vec2(lines_x[i], coord[i]), glm::vec2(lines_x[i + 1], coord[i + 1]), 0.007f);
            }
        }

        bool Update(InteractProxy const& proxy) {
            if (proxy.IsHovering()){
                glm::vec2 pos = proxy.MousePos();
                for (int i = 0; i < 7; ++i){
                    if (pos[0] >= lines_x[i] - 0.05f && pos[0] <= lines_x[i] + 0.05f && pos[1] >= lines_y[0] - 0.01f && pos[1] <= lines_y[1] + 0.01f){
                        
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                        
                        if (proxy.IsClicking()){
                            chosen = i;
                        }
                        
                        if (proxy.IsDragging()){
                            chosen = i;
                            glm::vec2 startPos = proxy.DraggingStartPoint();
                            _inf[i] = startPos[1] > pos[1] ? startPos[1] : pos[1];
                            _sup[i] = startPos[1] > pos[1] ? pos[1] : startPos[1];
                            switch (i){
                                case 0: 
                                    inf.cylinders = Max.cylinders - (_inf[i] - lines_y[0]) / (lines_y[1] - lines_y[0]) * (Max.cylinders - Min.cylinders);
                                    sup.cylinders = Min.cylinders + (lines_y[1] - _sup[i]) / (lines_y[1] - lines_y[0]) * (Max.cylinders - Min.cylinders);
                                    break;
                                case 1: 
                                    inf.displacement = Max.displacement - (_inf[i] - lines_y[0]) / (lines_y[1] - lines_y[0]) * (Max.displacement - Min.displacement);
                                    sup.displacement = Min.displacement + (lines_y[1] - _sup[i]) / (lines_y[1] - lines_y[0]) * (Max.displacement - Min.displacement);
                                    break;
                                case 2: 
                                    inf.weight = Max.weight - (_inf[i] - lines_y[0]) / (lines_y[1] - lines_y[0]) * (Max.weight - Min.weight);
                                    sup.weight = Min.weight + (lines_y[1] - _sup[i]) / (lines_y[1] - lines_y[0]) * (Max.weight - Min.weight);
                                    break;
                                case 3: 
                                    inf.horsepower = Max.horsepower - (_inf[i] - lines_y[0]) / (lines_y[1] - lines_y[0]) * (Max.horsepower - Min.horsepower);
                                    sup.horsepower = Min.horsepower + (lines_y[1] - _sup[i]) / (lines_y[1] - lines_y[0]) * (Max.horsepower - Min.horsepower);
                                    break;
                                case 4: 
                                    inf.acceleration = Max.acceleration - (_inf[i] - lines_y[0]) / (lines_y[1] - lines_y[0]) * (Max.acceleration - Min.acceleration);
                                    sup.acceleration = Min.acceleration + (lines_y[1] - _sup[i]) / (lines_y[1] - lines_y[0]) * (Max.acceleration - Min.acceleration);
                                    break;
                                case 5: 
                                    inf.mileage = Max.mileage - (_inf[i] - lines_y[0]) / (lines_y[1] - lines_y[0]) * (Max.mileage - Min.mileage);
                                    sup.mileage = Min.mileage + (lines_y[1] - _sup[i]) / (lines_y[1] - lines_y[0]) * (Max.mileage - Min.mileage);
                                    break;
                                case 6: 
                                    inf.year = Max.year - (_inf[i] - lines_y[0]) / (lines_y[1] - lines_y[0]) * (Max.year - Min.year);
                                    sup.year = Min.year + (lines_y[1] - _sup[i]) / (lines_y[1] - lines_y[0]) * (Max.year - Min.year);
                                    break;
                            }
                        }
                        return true;
                    }
                }
            }
            return false;
        }
        
        void Paint(Common::ImageRGB& input){
            SetBackGround(input, glm::vec4(1.f, 1.f, 1.f, 1.f));
            // Draw all the coords.
            for (int i = 0; i < 7; ++i){
                DrawLine(input, glm::vec4(0.f, 0.f, 0.f, 1.f), glm::vec2(lines_x[i], lines_y[0]), glm::vec2(lines_x[i], lines_y[1]), 0.007f);
            }
            // Print Text
            PrintText(input, text_color, glm::vec2(lines_x[0], lines_y[0] - line_height * line_shift_rate), line_height, "cylinders");
            PrintText(input, text_color, glm::vec2(lines_x[1], lines_y[0] - line_height * line_shift_rate), line_height, "displacement");
            PrintText(input, text_color, glm::vec2(lines_x[2], lines_y[0] - line_height * line_shift_rate), line_height, "weight");
            PrintText(input, text_color, glm::vec2(lines_x[3], lines_y[0] - line_height * line_shift_rate), line_height, "horsepower");
            PrintText(input, text_color, glm::vec2(lines_x[4], lines_y[0] - line_height * line_shift_rate), line_height, "acceleration (0-60 mph)");
            PrintText(input, text_color, glm::vec2(lines_x[5], lines_y[0] - line_height * line_shift_rate), line_height, "mileage");
            PrintText(input, text_color, glm::vec2(lines_x[6], lines_y[0] - line_height * line_shift_rate), line_height, "year");
            // Draw data
            for (int i = 0; i < data.size(); ++i){
                Drawdata(input, i);
            }
            // Draw all the rects.
            
            for (int i = 0; i < 7; ++i) {
                if (i!=chosen) {
                    DrawLine(input, glm::vec4(blue, .5f), glm::vec2(lines_x[i], _inf[i]), glm::vec2(lines_x[i], _sup[i]), line_width_rect);
                    DrawLine(input, glm::vec4(blue, .5f), glm::vec2(lines_x[i] + line_width_rect * 0.001f, _inf[i]), glm::vec2(lines_x[i] + line_width_rect * 0.001f, _sup[i]), line_width_rect);
                    DrawRect(input, glm::vec4(1, 1, 1, 1), glm::vec2(lines_x[i] - rect_width / 2, _inf[i]), glm::vec2(rect_width, _sup[i] - _inf[i]), rect_line_width);
                }
                else {
                    DrawLine(input, glm::vec4(red, 1.f), glm::vec2(lines_x[i], _inf[i]), glm::vec2(lines_x[i], _sup[i]), line_width_rect);
                    DrawLine(input, glm::vec4(red, 1.f), glm::vec2(lines_x[i] + line_width_rect * 0.001f, _inf[i]), glm::vec2(lines_x[i] + line_width_rect * 0.001f, _sup[i]), line_width_rect);
                    DrawRect(input, glm::vec4(1, 1, 1, 1), glm::vec2(lines_x[i] - rect_width / 2, _inf[i]), glm::vec2(rect_width, _sup[i] - _inf[i]), rect_line_width);
                }
                char ptr_a[20] = {}, ptr_b[20] = {};
                
                switch (i){
                    case 0:
                        sprintf(ptr_a, "%d", inf.cylinders);
                        sprintf(ptr_b, "%d", sup.cylinders);
                        break;
                    case 1:
                        sprintf(ptr_a, "%d sq in", (int)inf.displacement);
                        sprintf(ptr_b, "%d sq in", (int)sup.displacement);
                        break;
                    case 2:
                        sprintf(ptr_a, "%d lbs", (int)inf.weight);
                        sprintf(ptr_b, "%d lbs",(int) sup.weight);
                        break;
                    case 3:
                        sprintf(ptr_a, "%d hp", (int) inf.horsepower);
                        sprintf(ptr_b, "%d hp", (int) sup.horsepower);
                        break;
                    case 4:
                        sprintf(ptr_a, "%d sec", (int) inf.acceleration);
                        sprintf(ptr_b, "%d sec", (int) sup.acceleration);
                        break;
                    case 5:
                        sprintf(ptr_a, "%d mpg", (int) inf.mileage);
                        sprintf(ptr_b, "%d mpg", (int) sup.mileage);
                        break;
                    case 6:
                        sprintf(ptr_a, "%d", (int) inf.year);
                        sprintf(ptr_b, "%d", (int) sup.year);
                        break;
                }
                
                PrintText(input, text_color, glm::vec2(lines_x[i], _inf[i] + line_height * shift_rate), line_height, ptr_a);
                PrintText(input, text_color, glm::vec2(lines_x[i], _sup[i] - line_height * shift_rate), line_height, ptr_b);
            }
        }
    };

    bool PaintParallelCoordinates(Common::ImageRGB & input, InteractProxy const & proxy, std::vector<Car> const & data, bool force) {
        // your code here
        // for example: 
        //   static CoordinateStates states(data);
        //   SetBackGround(input, glm::vec4(1));
        //   ...
        static CoordinateStates states(data);                  // initialize
        bool                    change = states.Update(proxy); // update according to user input
        if (! force && ! change)
            return false;
        states.Paint(input);
        return true;
    }

    glm::vec3 compute_convolution(Common::ImageRGB const& noise,std::vector<glm::vec2>* array){
        glm::vec3 result(0);
        float     weight = 0;
        double    t      = 0;
        for (int i = 0; i < array->size(); ++i){
            int x = std::min((size_t) (*array)[i].x, noise.GetSizeX()-1), y = std::min((size_t) (*array)[i].y, noise.GetSizeY()-1);
            result += noise.At(std::max(x, 0), std::max(y, 0)) * glm::vec3(glm::pow(glm::cos(t-i*0.46), 2));
            weight += glm::pow(glm::cos(t - i * 0.46), 2);
        }
        return result / glm::vec3(weight);
    }

    void LIC(ImageRGB & output, Common::ImageRGB const & noise, VectorField2D const & field, int const & step) {
        // your code here
        for (int i = 0; i < output.GetSizeX(); ++i){
            for (int j = 0; j < output.GetSizeY(); ++j) {
                glm::vec2 tmp = glm::vec2(i, j);
                glm::vec2 vec = field.At(tmp.x, tmp.y);
                std::vector<glm::vec2> * curve = new std::vector<glm::vec2>(1, tmp);
                float x = tmp.x, y = tmp.y;
                for (int i = 0; i < step; ++i){
                    x += 2.f * vec.x;
                    y += 2.f * vec.y;
                    curve->push_back(glm::vec2(x, y));
                    vec = field.At((int) x, (int) y);
                }
                x = tmp.x, y = tmp.y, vec = field.At(tmp.x, tmp.y);
                for (int i = 0; i < step; ++i){
                    x -= 2.f * vec.x;
                    y -= 2.f * vec.y;
                    curve->push_back(glm::vec2(x, y));
                    vec = field.At((int) x, (int) y);
                }

                glm::vec3 result(0);
                float     weight = 0;
                double    t      = 0;
                for (int i = 0; i < curve->size(); ++i){
                    int x = std::min((size_t) (*curve)[i].x, noise.GetSizeX()-1), y = std::min((size_t) (*curve)[i].y, noise.GetSizeY()-1);
                    result += noise.At(std::max(x, 0), std::max(y, 0)) * glm::vec3(glm::pow(glm::cos(t - i * 0.46), 2));
                    weight += glm::pow(glm::cos(t - i * 0.46), 2);
                }

                output.At(i, j) = result / glm::vec3(weight);
            }
        }
    }
}; // namespace VCX::Labs::Visualization