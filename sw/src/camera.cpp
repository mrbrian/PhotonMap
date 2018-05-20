#include "camera.h"

Matrix4x4 *Camera::calc_proj_matrix()
{
    Matrix4x4 *result = new Matrix4x4();
    Matrix4x4 &m_projection = *result;

    m_projection[0][0] = 1 / (aspect * tan(fov / 2));
    m_projection[1][1] = 1 / (aspect * tan(fov / 2));
    m_projection[2][2] = (far + near) / (far - near);
    m_projection[2][3] = -2 * (far * near) / (far - near);
    m_projection[3][2] = 1;
    m_projection[3][3] = 0;

    return result;
}

Matrix4x4 Camera::get_view_matrix()
{
    return m_view;
}

Plane *Camera::calc_img_plane()
{
    Vector3D fwd = Vector3D(0,1,0).cross(lookAt);
    double x1 = std::tan(fov / 2) * near;
    double y1 = std::tan(fov / 2) * near;
    Plane *result = new Plane(
                Point3D(x1, y1, near),
                Point3D(x1, -y1, near),
                Point3D(-x1, -y1, near),
                Point3D(-x1, y1, near)
            );
    Matrix4x4 m_view = get_view_matrix();
    result->Transform(m_view);
    //delete(m_view);

    return result;
    // new Plane(Point3D(-img_plane_w, img_plane_w, -1), Point3D(-img_plane_w, -img_plane_w, -1), Point3D(img_plane_w, -img_plane_w, -1), Point3D(img_plane_w, img_plane_w, -1));
}
