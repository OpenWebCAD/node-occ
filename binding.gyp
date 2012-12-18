{
  "targets": [
    {
      "target_name": "occ",
	  "msvs_settings": {

				'VCLinkerTool': {					
					'AdditionalLibraryDirectories': [
							"/projects/oce-0.11/Win64/lib/",
					],
					# 'AdditionalOptions': ["/FORCE",],						
				},	  
	  },
		
      "sources": [
			"src/GeometryBuilder.cc",
			"src/Base.h",
			"src/Base.cc",
			"src/GeometryBuilder.h",
			"src/NodeV8.h",
			"src/Transformation.h",
			"src/Transformation.cc",
			"src/Solid.h",
			"src/Solid.cc",
			"src/Shape.h",
			"src/Shape.cc",
			"src/Vertex.h",
			"src/Vertex.cc",
			"src/Edge.h",
			"src/Edge.cc",
			"src/Wire.h",
			"src/Wire.cc",
			"src/Mesh.h",
			"src/Mesh.cc",
			"src/Tools.cc",
			"src/OCC.h"
			"src/Util.h",
			"src/Util.cc",
			"src/V8Wrapper.cc",
			],
			
	  "defines": [],
	
	  "include_dirs": [  
    	"/projects/oce-0.11/include/oce",
	    "/projet/oce/inc",			    ],				

	"libraries": [ 
		'TKAdvToolsd.lib',
		'TKBOd.lib',
		'TKBoold.lib',
		'TKBRepd.lib',
		'TKerneld.lib',
		'TKFeatd.lib',
		'TKFilletd.lib',
		'TKG2dd.lib',
		'TKG3dd.lib',
		'TKGeomAlgod.lib',
		'TKGeomBased.lib',
		'TKHLRd.lib',
		'TKMathd.lib',
		'TKMeshd.lib',
		'TKOffsetd.lib',
		'TKPrimd.lib',
		'TKShHealingd.lib',
		'TKTopAlgod.lib',
		'TKXMeshd.lib',
		'TKStepd.lib',
		'TKIGESd.lib',
		],
    }
  ]
}