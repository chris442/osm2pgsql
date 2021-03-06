#ifndef OSM2PGSQL_REPROJECTION_HPP
#define OSM2PGSQL_REPROJECTION_HPP

/*
 * Convert OSM latitude / longitude from degrees to mercator
 * so that Mapnik does not have to project the data again
 *
 */

#include <memory>

#include <osmium/geom/coordinates.hpp>
#include <osmium/osm/location.hpp>

enum Projection
{
    PROJ_LATLONG = 4326,
    PROJ_SPHERE_MERC = 3857
};

class reprojection
{
public:
    reprojection() = default;

    reprojection(reprojection const &) = delete;
    reprojection &operator=(reprojection const &) = delete;

    reprojection(reprojection &&) = delete;
    reprojection &operator=(reprojection &&) = delete;

    virtual ~reprojection() = default;

    /**
     * Reproject from the source projection lat/lon (EPSG:4326)
     * to target projection.
     */
    virtual osmium::geom::Coordinates reproject(osmium::Location loc) const = 0;

    /**
     * Converts coordinates from target projection to tile projection (EPSG:3857)
     *
     * Do not confuse with coords_to_tile which does *not* calculate coordinates in the
     * tile projection, but tile coordinates.
     */
    virtual void target_to_tile(double *lat, double *lon) const = 0;

    /**
     * Converts from target coordinates to tile coordinates.
     *
     * The zoom level for the coordinates is explicitly given in the
     * variable map_width.
     */
    void coords_to_tile(double *tilex, double *tiley, double lon, double lat,
                        int map_width);
    virtual int target_srs() const = 0;
    virtual char const *target_desc() const = 0;

    bool target_latlon() const { return target_srs() == PROJ_LATLONG; }

    /**
     * Create a reprojection object with target srs `srs`.
     *
     * The target projection (used in the PostGIS tables).
     * Controlled by the -l/-m/-E options.
     */
    static std::shared_ptr<reprojection> create_projection(int srs);

private:
    static std::shared_ptr<reprojection> make_generic_projection(int srs);
};

#endif // OSM2PGSQL_REPROJECTION_HPP
