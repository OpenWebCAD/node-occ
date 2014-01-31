{
  "targets": [
    {
      "target_name": "occ",
      "cflags!"    : [ '-fno-exceptions' ],
      "cflags_cc!" : [ '-fno-exceptions' ],
      "cflags"     : [ '-Wno-ignored-qualifiers', '-Wno-unused-variable', '-Wno-reorder' ],
      "cflags_cc"  : [ '-Wno-ignored-qualifiers', '-Wno-unused-variable', '-Wno-reorder' ],
      'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
      },
	  "msvs_settings": {

		'VCLinkerTool': {					
			'AdditionalLibraryDirectories': [
			    "C:/oce-0.13/Win64/lib",
			],
			# 'AdditionalOptions': ["/FORCE",],						
		},	  
	  },
		
      "sources": [
			"src/Base.h",
			"src/Base.cc",
			"src/BooleanOperation.h",
			"src/BooleanOperation.cc",
			"src/BoundingBox.h",
			"src/BoundingBox.cc",
			"src/Edge.h",
			"src/Edge.cc",
			"src/Face.h",
			"src/Face.cc",
			"src/GeometryBuilder.cc",
			"src/GeometryBuilder.h",
			"src/Mesh.h",
			"src/Mesh.cc",
			"src/NodeV8.h",
			"src/OCC.h",
			"src/Point3Wrap.h",
			"src/Point3Wrap.cc",
			"src/Shape.h",
			"src/Shape.cc",
			"src/ShapeFactory.h",
			"src/ShapeFactory.cc",
			"src/ShapeIterator.h",
			"src/ShapeIterator.cc",
			"src/Shell.h",
			"src/Shell.cc",
			"src/Solid.h",
			"src/Solid.cc",
			"src/Tools.h",
			"src/Tools.cc",
			"src/Transformation.h",
			"src/Transformation.cc",
			"src/Util.h",
			"src/Util.cc",
			"src/Vertex.h",
			"src/Vertex.cc",
			"src/Wire.h",
			"src/Wire.cc",
			"src/V8Wrapper.cc",
			],
			
	  "defines": [
		 "OCE_HAVE_IOMANIP",
		 "OCE_HAVE_IOSTREAM",
		 "OCE_HAVE_CLIMITS" 
	  ],

	  "library_dirs": [
	     "/usr/local/libs",
	     "c:/OCE-0.13/Win64/Bin"
	  ],
	  "include_dirs": [  
           "/usr/local/include/oce",
	       "c:/OCE-0.13/include/oce",		   
	  ],				

	  "libraries": [ 
		'-lTKAdvTools',
		'-lTKBO',
		'-lTKBool',
		'-lTKBRep',
		'-lTKernel',
		'-lTKFeat',
		'-lTKFillet',
		'-lTKG2d',
		'-lTKG3d',
		'-lTKGeomAlgo',
		'-lTKGeomBase',
		'-lTKIGES',
		'-lTKernel',
		'-lTKMath',
		'-lTKMesh',
		'-lTKOffset',
		'-lTKPrim',
		'-lTKPShape',
		'-lTKShHealing',
		'-lTKTopAlgo',
		'-lTKIGES',
		'-lTKSTEP',
		'-lTKSTEPAttr',
		'-lTKSTEPBase',
		'-lTKFillet',
		'-lTKXSBase',
		'-lTKSTL',
		'-lTKTObj',
		'-lTKLCAF',
		'-lTKCAF'
	],
    }
  ]
}
