#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <spdlog/spdlog.h>
#include <iostream>
#include "Labs/4-Animation/tasks.h"
#include "IKSystem.h"
#include "CustomFunc.inl"


namespace VCX::Labs::Animation {
    void ForwardKinematics(IKSystem & ik, int StartIndex) {
        if (StartIndex == 0) {
            ik.JointGlobalRotation[0] = ik.JointLocalRotation[0];
            ik.JointGlobalPosition[0] = ik.JointLocalOffset[0];
            StartIndex                = 1;
        }
        
        for (int i = StartIndex; i < ik.JointLocalOffset.size(); i++) {
            // your code here: forward kinematics, update JointGlobalPosition and JointGlobalRotation
            ik.JointGlobalRotation[i] = ik.JointGlobalRotation[i - 1] * ik.JointLocalRotation[i];
            glm::vec3 offset = ik.JointGlobalRotation[i - 1] * ik.JointLocalOffset[i];
            ik.JointGlobalPosition[i] = ik.JointGlobalPosition[i - 1] + ik.JointOffsetLength[i] * glm::normalize(glm::vec3(offset.x, offset.y, offset.z));
        }
    }

    void InverseKinematicsCCD(IKSystem & ik, const glm::vec3 & EndPosition, int maxCCDIKIteration, float eps) {
        ForwardKinematics(ik, 0);
        // These functions will be useful: glm::normalize, glm::rotation, glm::quat * glm::quat
        for (int CCDIKIteration = 0; CCDIKIteration < maxCCDIKIteration && glm::l2Norm(ik.EndEffectorPosition() - EndPosition) > eps; CCDIKIteration++) {
            // your code here: ccd ik
            for (int i = ik.NumJoints() - 2 ; i >= 0; --i){
                ik.JointLocalRotation[i + 1] = ik.JointLocalRotation[i + 1] * glm::rotation(glm::normalize(ik.JointGlobalPosition[ik.NumJoints() - 1] - ik.JointGlobalPosition[i]),glm::normalize( EndPosition - ik.JointGlobalPosition[i]));
                ForwardKinematics(ik, i+1);
            }
        }
    }

    void InverseKinematicsFABR(IKSystem & ik, const glm::vec3 & EndPosition, int maxFABRIKIteration, float eps) {
        ForwardKinematics(ik, 0);
        int nJoints = ik.NumJoints();
        std::vector<glm::vec3> backward_positions(nJoints, glm::vec3(0, 0, 0)), forward_positions(nJoints, glm::vec3(0, 0, 0));
        for (int IKIteration = 0; IKIteration < maxFABRIKIteration && glm::l2Norm(ik.EndEffectorPosition() - EndPosition) > eps; IKIteration++) {
            // task: fabr ik
            // backward update
            glm::vec3 next_position         = EndPosition;
            backward_positions[nJoints - 1] = EndPosition;

            for (int i = nJoints - 2; i >= 0; i--) {
                // your code here
                float lambda          = ik.JointOffsetLength[i + 1] / glm::l2Norm(backward_positions[i + 1] - ik.JointGlobalPosition[i]);
                backward_positions[i] = (1-lambda)*backward_positions[i + 1] + lambda*ik.JointGlobalPosition[i];
            }

            // forward update
            glm::vec3 now_position = ik.JointGlobalPosition[0];
            forward_positions[0] = ik.JointGlobalPosition[0];
            for (int i = 0; i < nJoints - 1; i++) {
                // your code here
                float lambda = ik.JointOffsetLength[i + 1] / glm::l2Norm(backward_positions[i + 1] - forward_positions[i]);
                forward_positions[i + 1] = lambda * backward_positions[i + 1] + (1 - lambda) * forward_positions[i];
            }
            ik.JointGlobalPosition = forward_positions; // copy forward positions to joint_positions
        }

        // Compute joint rotation by position here.
        for (int i = 0; i < nJoints - 1; i++) {
            ik.JointGlobalRotation[i] = glm::rotation(glm::normalize(ik.JointLocalOffset[i + 1]), glm::normalize(ik.JointGlobalPosition[i + 1] - ik.JointGlobalPosition[i]));
        }
        ik.JointLocalRotation[0] = ik.JointGlobalRotation[0];
        for (int i = 1; i < nJoints - 1; i++) {
            ik.JointLocalRotation[i] = glm::inverse(ik.JointGlobalRotation[i - 1]) * ik.JointGlobalRotation[i];
        }
        ForwardKinematics(ik, 0);
    }

    std::vector<glm::vec3> ResampleByArcLength(const std::vector<glm::vec3>& points, int targetSamples) {
        std::vector<float> arcLengths(points.size(), 0.0f);

        float totalLength = 0.0f;
        for (int i = 1; i < points.size(); ++i) {
            totalLength += glm::length(points[i] - points[i - 1]);
            arcLengths[i] = arcLengths[i - 1] + glm::length(points[i] - points[i - 1]);
        }

        float segmentLength = totalLength / (targetSamples - 1);
        float threshold = segmentLength * 1000.0f;
        std::vector<glm::vec3> resampledPoints;
        resampledPoints.push_back(points.front());
        
        float currentLength = segmentLength;
        for (int i = 1; i < targetSamples - 1; ++i) {
            for (int j = 1; j < arcLengths.size(); ++j) {
                if (arcLengths[j] >= currentLength) {
                    if(arcLengths[j] - arcLengths[j - 1] > threshold || arcLengths[j] - arcLengths[j - 1] < 1e-6)continue;
                    float alpha = (currentLength - arcLengths[j - 1]) / (arcLengths[j] - arcLengths[j - 1]);
                    resampledPoints.push_back(glm::mix(points[j - 1], points[j], alpha));
                    currentLength += segmentLength;
                    break;
                }
            }
        }
        if (resampledPoints.back() != points.front())
            resampledPoints.push_back(points.front());
        return resampledPoints;
    }
/*
    IKSystem::Vec3ArrPtr IKSystem::BuildCustomTargetPosition() {
        // get function from https://www.wolframalpha.com/input/?i=Albert+Einstein+curve
        int nums = 5000;
        using Vec3Arr = std::vector<glm::vec3>;
        std::shared_ptr<Vec3Arr> raw(new Vec3Arr(nums));
        int index = 0;
        for (int i = 0; i < nums; i++) {
            float x_val = 1.5e-3f * custom_x(92 * glm::pi<float>() * i / nums);
            float y_val = 1.5e-3f * custom_y(92 * glm::pi<float>() * i / nums);
            if (std::abs(x_val) < 1e-3 || std::abs(y_val) < 1e-3) continue;
            (*raw)[index++] = glm::vec3(1.6f - x_val, 0.0f, y_val - 0.2f);
        }
        raw->resize(index);
        std::shared_ptr<Vec3Arr> custom(new Vec3Arr(ResampleByArcLength(*raw, 5000)));
        return custom;
    }
*/
    float px (float t){
        if (t < 5.0f) {
            return 3.0f;
        } else {
            float angle = (t - 5.0f) / 5.0f * EIGEN_PI;
            return 3.0f - std::sin(angle);
        }
    }

    float py (float t){
        if (t < 5.0f) {
            return t / 5.0f * 4.0f;
        } else {
            float angle = (t - 5.0f) / 5.0f * EIGEN_PI;
            return 3.0f + std::cos(angle);
        }
    }

    IKSystem::Vec3ArrPtr IKSystem::BuildCustomTargetPosition() {
        int nums = 5000;
        using Vec3Arr = std::vector<glm::vec3>;
        std::shared_ptr<Vec3Arr> custom(new Vec3Arr(nums));
        int index = 0;
        for (int i = 0; i < nums; i++) {
            float x_val = 1.5e-1f * px((float)i / 500.0f);
            float y_val = 1.5e-1f * py((float)i / 500.0f);
            if (std::abs(x_val) < 1e-3 || std::abs(y_val) < 1e-3) continue;
            (*custom)[index++] = glm::vec3(x_val, 0.0f, y_val);
        }
        custom->resize(index);
        return custom;
    }

    static Eigen::VectorXf glm2eigen(std::vector<glm::vec3> const & glm_v) {
        Eigen::VectorXf v = Eigen::Map<Eigen::VectorXf const, Eigen::Aligned>(reinterpret_cast<float const *>(glm_v.data()), static_cast<int>(glm_v.size() * 3));
        return v;
    }

    static std::vector<glm::vec3> eigen2glm(Eigen::VectorXf const & eigen_v) {
        return std::vector<glm::vec3>(
            reinterpret_cast<glm::vec3 const *>(eigen_v.data()),
            reinterpret_cast<glm::vec3 const *>(eigen_v.data() + eigen_v.size())
        );
    }

    static Eigen::SparseMatrix<float> CreateEigenSparseMatrix(std::size_t n, std::vector<Eigen::Triplet<float>> const & triplets) {
        Eigen::SparseMatrix<float> matLinearized(n, n);
        matLinearized.setFromTriplets(triplets.begin(), triplets.end());
        return matLinearized;
    }

    // solve Ax = b and return x
    static Eigen::VectorXf ComputeSimplicialLLT(
        Eigen::SparseMatrix<float> const & A,
        Eigen::VectorXf const & b) {
        auto solver = Eigen::SimplicialLLT<Eigen::SparseMatrix<float>>(A);
        return solver.solve(b);
    }

/*
    void AdvanceMassSpringSystem(MassSpringSystem & system, float const dt) {
        // your code here: rewrite following code
        int const steps = 1000;
        float const ddt = dt / steps; 
        for (std::size_t s = 0; s < steps; s++) {
            std::vector<glm::vec3> forces(system.Positions.size(), glm::vec3(0));
            for (auto const spring : system.Springs) {
                auto const p0 = spring.AdjIdx.first;
                auto const p1 = spring.AdjIdx.second;
                glm::vec3 const x01 = system.Positions[p1] - system.Positions[p0];
                glm::vec3 const v01 = system.Velocities[p1] - system.Velocities[p0];
                glm::vec3 const e01 = glm::normalize(x01);
                glm::vec3 f = (system.Stiffness * (glm::length(x01) - spring.RestLength) + system.Damping * glm::dot(v01, e01)) * e01;
                forces[p0] += f;
                forces[p1] -= f;
            }
            for (std::size_t i = 0; i < system.Positions.size(); i++) {
                if (system.Fixed[i]) continue;
                system.Velocities[i] += (glm::vec3(0, -system.Gravity, 0) + forces[i] / system.Mass) * ddt;
                system.Positions[i] += system.Velocities[i] * ddt;
            }
        }
    }
*/
    void AdvanceMassSpringSystem(MassSpringSystem & system, float const dt) {
        // your code here: rewrite following code
        int const   steps = 1;
        float const ddt   = dt / steps;
        int const   N = system.Positions.size();
        for (std::size_t s = 0; s < steps; s++) {
            std::vector<glm::vec3>          forces(N, glm::vec3(0));
            std::vector<std::vector<float>> tempH(3*N, std::vector<float>(3*N, 0));

            for (auto const spring : system.Springs) {
                auto const      p0  = spring.AdjIdx.first;
                auto const      p1  = spring.AdjIdx.second;
                glm::vec3 const x01 = system.Positions[p1] - system.Positions[p0];
                glm::vec3 const v01 = system.Velocities[p1] - system.Velocities[p0];
                glm::vec3 const e01 = glm::normalize(x01);
                glm::vec3       f   = (system.Stiffness * (glm::length(x01) - spring.RestLength) + system.Damping * glm::dot(v01, e01)) * e01;
                forces[p0] += f;
                forces[p1] -= f;
                // the following for computing tempH
                glm::mat3 tmp = glm::outerProduct(e01, e01);

                glm::mat3 He = system.Stiffness * tmp + system.Stiffness * (1 - spring.RestLength / glm::length(x01)) * (glm::mat3(1.0) - tmp);
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        tempH[3 * p0 + i][3 * p0 + j] += He[i][j];
                        tempH[3 * p1 + i][3 * p1 + j] += He[i][j];
                        tempH[3 * p0 + i][3 * p1 + j] -= He[i][j];
                        tempH[3 * p1 + i][3 * p0 + j] -= He[i][j];
                    }
                }
            }
            float tmp = system.Mass / (ddt * ddt);
            for (int i = 0; i < 3*N; i++) tempH[i][i] += tmp;

            std::vector<Eigen::Triplet<float>> triplets;
            for (int i = 0; i < 3*N; i++)
                for (int j = 0; j < 3*N; j++) triplets.emplace_back(i, j, tempH[i][j]);

            auto Hg = CreateEigenSparseMatrix(3*N, triplets);

            std::vector<glm::vec3> y_glm = system.Velocities;
            for (int i = 0; i < N; i++) y_glm[i] += glm::vec3(0, -system.Gravity * ddt, 0);
            auto y      = glm2eigen(y_glm);
            auto grad_E = -glm2eigen(forces);
            auto grad_g = -y * system.Mass / ddt + grad_E; 

            auto delta_x     = ComputeSimplicialLLT(Hg, -grad_g);
            auto delta_x_glm = eigen2glm(delta_x);

            for (std::size_t i = 0; i < N; i++) {
                if (system.Fixed[i]) continue;
                system.Velocities[i] = delta_x_glm[i] / ddt;
                system.Positions[i] += delta_x_glm[i];
            }
        }
    }
}
