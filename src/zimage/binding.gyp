{
  "targets": [
    {
      "target_name": "zimage",
      "sources": ["src/zimage.cpp"],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "<(module_root_dir)",
        "<(module_root_dir)/include",
        "<(module_root_dir)/include/glib-2.0",
        "<(module_root_dir)/lib/glib-2.0/include"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"],
      "conditions": [
        ["OS=='linux'", {
          "cflags_cc": [
            "-std=c++17",
            "-O2",
            "-fexceptions"
          ],
          "libraries": [
            "-L<(module_root_dir)/lib"
          ],
          "ldflags": [
            "-Wl,-rpath,$$ORIGIN",
            "-Wl,-z,origin"
          ]
        }]
      ]
    }
  ]
}