#include <I_Scene.h>
#include <photon.h>
#include <PhotonSceneRenderer.h>

PhotonSceneRenderer::PhotonSceneRenderer(I_Scene &scene)
	: scene_(scene)
{
}

PhotonSceneRenderer::~PhotonSceneRenderer()
{

}

Color *PhotonSceneRenderer::Render()
{
    Color * result = new Color[cam()->imgWidth * cam()->imgHeight];
    // iterate over the pixels & set colour values

    for (int x = 0; x < cam()->imgWidth; x++)
    {
        for (int y = 0; y < cam()->imgHeight; y++)
        {
            result[x + y * cam()->imgWidth] = Color(0, 0, 0);  // initialize to all black
        }
    }

    for(std::vector<photon*>::iterator it = photon_map->begin(); it != photon_map->end(); ++it)
    {
        photon *p = (*it);

        // calc image space coordinates
        Point2D img_coords = calc_image_coords(p->get_position());
        int x = (int)img_coords[0];
        int y = (int)img_coords[1];
        int idx = x + y * cam()->imgWidth;
        if (idx < 0 || idx >= cam()->imgHeight * cam()->imgWidth)
            continue;
        Color &pixel = result[x + y * cam()->imgWidth];
        pixel = pixel + *(p->get_color());
    }

    return result;
}

int PhotonSceneRenderer::imageWidth()
{
	return imageWidth_;
}

int PhotonSceneRenderer::imageHeight()
{
	return imageHeight_;
}

Camera *PhotonSceneRenderer::cam()
{
    return scene_.cam();
}

Point2D PhotonSceneRenderer::calc_image_coords(Point3D pt)
{
    Matrix4x4 m_view = cam()->get_view_matrix();
    Matrix4x4 *m_projection = cam()->calc_proj_matrix();

    float width = cam()->imgWidth;
    float height = cam()->imgHeight;

    Matrix4x4 m_screenCoords = Matrix4x4();           // make transform for converting NDC space to screenspace
    m_screenCoords[0][0] = -width / 2;
    m_screenCoords[1][1] = height / 2;
    m_screenCoords = Matrix4x4::translation(Vector3D(width / 2, height / 2, 0)) * m_screenCoords;

    // Apply the view matrix
    pt = m_view * pt;

    // Do clipping here...
    //bool skipLine = false;
    //Point3D pt_i;

    //do_clip();

    // Apply the projection matrix to 4D points
    Point4D a_4d = Point4D(pt);

    a_4d = (*m_projection) * a_4d;

    // homogenization & store back into 3D points
    pt = Point3D(a_4d[0] / a_4d[3], a_4d[1] / a_4d[3], a_4d[2] / a_4d[3]);

    // map to viewport
    pt = m_screenCoords * pt;

    //delete(m_view);
    delete(m_projection);

    return Point2D(pt[0], pt[1]);
}
