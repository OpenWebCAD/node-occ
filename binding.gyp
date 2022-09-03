{
  "variables": {
    "dbg%": "",
    "OOO%": "\$${ORIGIN}"
  },
  "targets": [
    {
      "target_name": "occ",
         "conditions": [
              [
                "OS=='mac'",
                {
                   "xcode_settings": {
                        "OTHER_CFLAGS" : [
                            "-O3",
                            "-frtti",
                            "-Wno-ignored-qualifiers",
                            "-Wno-unused-variable",
                            "-Wno-reorder",
                            "-Wno-extra"
                        ],
                        "OTHER_LDFLAGS" : [

                        ]
                   },
                    "library_dirs": [
                    ],
                    "include_dirs": [
                      "/usr/include/opencascade",
                      "<!(node -e \"require('nan')\")"
                    ],},
              ],
              [
                "OS=='linux'",
                {
                      "cflags!": [
                        "-fno-exceptions"
                      ],
                      "cflags_cc!": [
                        "-fno-exceptions"
                      ],
                      "cflags": [
                        "-O3",
                        "-frtti",
                        "-Wno-ignored-qualifiers",
                        "-Wno-unused-variable",
                        "-Wno-reorder",
                        "-Wno-extra"
                      ],
                      "cflags_cc": [
                        "-O3",
                        "-frtti",
                        "-Wno-ignored-qualifiers",
                        "-Wno-unused-variable",
                        "-Wno-reorder",
                        "-Wno-extra"
                      ],
                      "ldflags": [
                        "-Wl,-rpath,\$$ORIGIN",
                        ],
                      "library_dirs": [
                        "/usr/lib/x86_64-linux-gnu",
                      ],
                      "include_dirs": [
                      "/usr/include/opencascade",
                        "<!(node -e \"require('nan')\")"
                      ],
                      "_libraries": [
                        "/usr/lib/x86_64-linux-gnu/libTKMath.so",
                        "/usr/lib/x86_64-linux-gnu/libTKG2d.so"
                      ]
                      }
              ],
              [
                "OS=='win'",
                {
                      "library_dirs": [
                        "./occt-7.5.0/win64/vc14/lib",
                      ],
                      "include_dirs": [
                        "./occt-7.5.0/inc",
                        "<!(node -e \"require('nan')\")"
                      ],}
              ]
         ],

      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
      },
      "msvs_settings": {
      },
      "sources": [
        "src/all.cc"
      ],
      "_sources": [
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
      "libraries+": [
        "-lTKBO<(dbg)",
        "-lTKMath<(dbg)",
        "-lTKBool<(dbg)",
        "-lTKBRep<(dbg)",
        "-lTKernel<(dbg)",
        "-lTKFeat<(dbg)",
        "-lTKFillet<(dbg)",
        "-lTKG2d<(dbg)",
        "-lTKG3d<(dbg)",
        "-lTKGeomAlgo<(dbg)",
        "-lTKGeomBase<(dbg)",
        "-lTKIGES<(dbg)",
        "-lTKernel<(dbg)",
        "-lTKMesh<(dbg)",
        "-lTKOffset<(dbg)",
        "-lTKPrim<(dbg)",
        "-lTKShHealing<(dbg)",
        "-lTKTopAlgo<(dbg)",
        "-lTKSTEP<(dbg)",
        "-lTKSTEPAttr<(dbg)",
        "-lTKSTEPBase<(dbg)",
        "-lTKSTEP209<(dbg)",
        "-lTKFillet<(dbg)",
        "-lTKFeat<(dbg)",
        "-lTKXSBase<(dbg)",
        "-lTKSTL<(dbg)",
      ],
      "other_libraries": [
        "-lTKTObj<(dbg)",
        "-lTKMath<(dbg)",
        "-lTKLCAF<(dbg)"
      ],
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [
        "<(module_name)"
      ],
      "copies": [
        {
          "files": [
            "<(PRODUCT_DIR)/<(module_name).node",
          ],
          "destination": "<(module_path)"
        }
      ]
    },
    {
      "target_name": "action_after_build1",
      "type": "none",
      "dependencies": [
        "<(module_name)"
      ],
      "conditions": [

        ["OS=='win'",
        {
          "variables": {
             "bin_folder": 'occt-7.2.0/win64/vc14/bin'
          },
          "copies": [
            {
              "files": [
                "<(bin_folder)/TKBO.dll",
                "<(bin_folder)/TKBool.dll",
                "<(bin_folder)/TKBRep.dll",
                "<(bin_folder)/TKernel.dll",
                "<(bin_folder)/TKFillet.dll",
                "<(bin_folder)/TKG2d.dll",
                "<(bin_folder)/TKG3d.dll",
                "<(bin_folder)/TKGeomAlgo.dll",
                "<(bin_folder)/TKGeomBase.dll",
                "<(bin_folder)/TKMath.dll",
                "<(bin_folder)/TKMesh.dll",
                "<(bin_folder)/TKOffset.dll",
                "<(bin_folder)/TKPrim.dll",
                "<(bin_folder)/TKShHealing.dll",
                "<(bin_folder)/TKSTEP.dll",
                "<(bin_folder)/TKSTEP209.dll",
                "<(bin_folder)/TKSTEPAttr.dll",
                "<(bin_folder)/TKSTEPBase.dll",
                "<(bin_folder)/TKSTL.dll",
                "<(bin_folder)/TKTopAlgo.dll",
                "<(bin_folder)/TKXSBase.dll",
              ],
              "unused_files": [
                "<(bin_folder)/FWOSPlugin.dll",
                "<(bin_folder)/FWOSPlugind.dll",
                "<(bin_folder)/PTKernel.dll",
                "<(bin_folder)/PTKerneld.dll",
                "<(bin_folder)/TKBinL.dll",
                "<(bin_folder)/TKBinLd.dll",
                "<(bin_folder)/TKBOd.dll",
                "<(bin_folder)/TKBoold.dll",
                "<(bin_folder)/TKBRepd.dll",
                "<(bin_folder)/TKCDF.dll",
                "<(bin_folder)/TKCDFd.dll",
                "<(bin_folder)/TKerneld.dll",
                "<(bin_folder)/TKFeat.dll",
                "<(bin_folder)/TKFeatd.dll",
                "<(bin_folder)/TKFilletd.dll",
                "<(bin_folder)/TKG2dd.dll",
                "<(bin_folder)/TKG3dd.dll",
                "<(bin_folder)/TKGeomAlgod.dll",
                "<(bin_folder)/TKGeomBased.dll",
                "<(bin_folder)/TKHLR.dll",
                "<(bin_folder)/TKHLRd.dll",
                "<(bin_folder)/TKIGES.dll",
                "<(bin_folder)/TKIGESd.dll",
                "<(bin_folder)/TKLCAF.dll",
                "<(bin_folder)/TKLCAFd.dll",
                "<(bin_folder)/TKMathd.dll",
                "<(bin_folder)/TKMeshd.dll",
                "<(bin_folder)/TKOffsetd.dll",
                "<(bin_folder)/TKPLCAF.dll",
                "<(bin_folder)/TKPLCAFd.dll",
                "<(bin_folder)/TKPrimd.dll",
                "<(bin_folder)/TKPShape.dll",
                "<(bin_folder)/TKPShaped.dll",
                "<(bin_folder)/TKShapeSchema.dll",
                "<(bin_folder)/TKShapeSchemad.dll",
                "<(bin_folder)/TKShHealingd.dll",
                "<(bin_folder)/TKStdLSchema.dll",
                "<(bin_folder)/TKStdLSchemad.dll",
                "<(bin_folder)/TKSTEP209d.dll",
                "<(bin_folder)/TKSTEPAttrd.dll",
                "<(bin_folder)/TKSTEPBased.dll",
                "<(bin_folder)/TKSTEPd.dll",
                "<(bin_folder)/TKSTLd.dll",
                "<(bin_folder)/TKTObj.dll",
                "<(bin_folder)/TKTObjd.dll",
                "<(bin_folder)/TKTopAlgod.dll",
                "<(bin_folder)/TKXMesh.dll",
                "<(bin_folder)/TKXMeshd.dll",
                "<(bin_folder)/TKXmlL.dll",
                "<(bin_folder)/TKXmlLd.dll",
                "<(bin_folder)/TKXSBased.dll"
              ],
              "destination": "<(module_path)"
            }
          ]
        }]
      ]
    }
  ]
}
