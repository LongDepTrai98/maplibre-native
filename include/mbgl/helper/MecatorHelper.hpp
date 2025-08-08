#pragma once 
#include <cmath>
namespace mbgl
{
	namespace gl
	{
		class MecatorHelper
		{
        public: 
			static double computeScaleZForLevel(const int& zoomlevel, const double& tileSizeInUnits = 8192.0f)
			{
				const double pi = 3.141592653589793238462643383279502884197169;
                const double earth_radius = 6378137.0;  
				double earthCircumference = 2 * pi * earth_radius; 
				uint64_t numTiles = 1 << zoomlevel; 
				double tileSizeInMeters = earthCircumference / numTiles;
                double scale = tileSizeInUnits / tileSizeInMeters; 
				double scaleZ = 1.0 / scale; 
				return scaleZ; 
			}
			static double pixelToMeter(const double& scaleZ, const double& pixelZ)
			{
				return scaleZ * pixelZ; 
			}
            static double solveZoomLevelFor1MeterPerPixel(const double& targetMetersPerPixel = 1.0,int tileSize = 512) {
				const double pi = 3.141592653589793238462643383279502884197169;
				const double earth_radius = 6378137.0;
				double earthCircumference = 2 * pi * earth_radius;
				double worldSizePixels = earthCircumference / targetMetersPerPixel;
				double zoom = std::log2(worldSizePixels / tileSize);
				return zoom;
            }
        private: 
			MecatorHelper(); 
		};
	}
}