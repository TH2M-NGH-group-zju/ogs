/**
 * \file
 * \copyright
 * Copyright (c) 2012-2020, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

namespace GeoLib
{
template <typename InputIterator>
std::pair<Eigen::Vector3d, double> getNewellPlane(InputIterator pnts_begin,
                                                  InputIterator pnts_end)
{
    Eigen::Vector3d plane_normal({0, 0, 0});
    Eigen::Vector3d centroid({0, 0, 0});
    for (auto i=std::prev(pnts_end), j=pnts_begin; j!=pnts_end; i = j, ++j) {
        auto &pt_i = *(*i);
        auto &pt_j = *(*j);
        plane_normal[0] += (pt_i[1] - pt_j[1])
                           * (pt_i[2] + pt_j[2]); // projection on yz
        plane_normal[1] += (pt_i[2] - pt_j[2])
                           * (pt_i[0] + pt_j[0]); // projection on xz
        plane_normal[2] += (pt_i[0] - pt_j[0])
                           * (pt_i[1] + pt_j[1]); // projection on xy

        centroid += Eigen::Map<Eigen::Vector3d const>(pt_j.getCoords());
    }

    plane_normal.normalize();
    auto n_pnts(std::distance(pnts_begin, pnts_end));
    assert(n_pnts > 2);
    double d = 0.0;
    if (n_pnts > 0)
    {
        d = centroid.dot(plane_normal) / n_pnts;
    }
    return std::make_pair(plane_normal, d);
}

template <class T_POINT>
std::pair<Eigen::Vector3d, double> getNewellPlane(
    const std::vector<T_POINT*>& pnts)
{
    return getNewellPlane(pnts.begin(), pnts.end());
}

template <class T_POINT>
std::pair<Eigen::Vector3d, double> getNewellPlane(
    const std::vector<T_POINT>& pnts)
{
    Eigen::Vector3d plane_normal({0, 0, 0});
    Eigen::Vector3d centroid({0, 0, 0});
    std::size_t n_pnts(pnts.size());
    for (std::size_t i = n_pnts - 1, j = 0; j < n_pnts; i = j, j++) {
        plane_normal[0] += (pnts[i][1] - pnts[j][1])
                           * (pnts[i][2] + pnts[j][2]); // projection on yz
        plane_normal[1] += (pnts[i][2] - pnts[j][2])
                           * (pnts[i][0] + pnts[j][0]); // projection on xz
        plane_normal[2] += (pnts[i][0] - pnts[j][0])
                           * (pnts[i][1] + pnts[j][1]); // projection on xy

        centroid += Eigen::Map<Eigen::Vector3d const>(pnts[j].getCoords());
    }

    plane_normal.normalize();
    double const d = centroid.dot(plane_normal) / n_pnts;
    return std::make_pair(plane_normal, d);
}

template<class T_MATRIX>
void compute2DRotationMatrixToX(MathLib::Vector3 const& v,
        T_MATRIX & rot_mat)
{
    const double cos_theta = v[0];
    const double sin_theta = v[1];
    rot_mat(0,0) = rot_mat(1,1) = cos_theta;
    rot_mat(0,1) = sin_theta;
    rot_mat(1,0) = -sin_theta;
    rot_mat(0,2) = rot_mat(1,2) = rot_mat(2,0) = rot_mat(2,1) = 0.0;
    rot_mat(2,2) = 1.0;
}

template <class T_MATRIX>
void compute3DRotationMatrixToX(MathLib::Vector3  const& v,
        T_MATRIX & rot_mat)
{
    // a vector on the plane
    MathLib::Vector3 yy(0, 0, 0);
    if (std::abs(v[0]) > 0.0 && std::abs(v[1]) + std::abs(v[2]) < std::numeric_limits<double>::epsilon()) {
        yy[2] = 1.0;
    } else if (std::abs(v[1]) > 0.0 && std::abs(v[0]) + std::abs(v[2]) < std::numeric_limits<double>::epsilon()) {
        yy[0] = 1.0;
    } else if (std::abs(v[2]) > 0.0 && std::abs(v[0]) + std::abs(v[1]) < std::numeric_limits<double>::epsilon()) {
        yy[1] = 1.0;
    } else {
        for (unsigned i = 0; i < 3; i++) {
            if (std::abs(v[i]) > 0.0) {
                yy[i] = -v[i];
                break;
            }
        }
    }
    // z"_vec
    MathLib::Vector3 zz(MathLib::crossProduct(v, yy));
    zz.normalize();
    // y"_vec
    yy = MathLib::crossProduct(zz, v);
    yy.normalize();

    for (unsigned i=0; i<3; ++i) {
        rot_mat(0, i) = v[i];
        rot_mat(1, i) = yy[i];
        rot_mat(2, i) = zz[i];
    }
}

template <typename InputIterator>
void rotatePoints(Eigen::Matrix3d const& rot_mat, InputIterator pnts_begin,
                  InputIterator pnts_end)
{
    for (auto it = pnts_begin; it != pnts_end; ++it)
    {
        Eigen::Map<Eigen::Vector3d>((*it)->getCoords()) =
            rot_mat * Eigen::Map<Eigen::Vector3d const>((*it)->getCoords());
    }
}

template <typename InputIterator1, typename InputIterator2>
Eigen::Matrix3d rotatePointsToXY(InputIterator1 p_pnts_begin,
                                 InputIterator1 p_pnts_end,
                                 InputIterator2 r_pnts_begin,
                                 InputIterator2 r_pnts_end)
{
    assert(std::distance(p_pnts_begin, p_pnts_end) > 2);

    // compute the plane normal
    auto const [plane_normal, d] =
        GeoLib::getNewellPlane(p_pnts_begin, p_pnts_end);

    // rotate points into x-y-plane
    Eigen::Matrix3d const rot_mat = computeRotationMatrixToXY(plane_normal);
    rotatePoints(rot_mat, r_pnts_begin, r_pnts_end);

    for (auto it = r_pnts_begin; it != r_pnts_end; ++it)
    {
        (*(*it))[2] = 0.0;  // should be -= d but there are numerical errors
    }

    return rot_mat;
}

template <typename P>
void rotatePoints(Eigen::Matrix3d const& rot_mat, std::vector<P*> const& pnts)
{
    rotatePoints(rot_mat, pnts.begin(), pnts.end());
}

} // end namespace GeoLib

