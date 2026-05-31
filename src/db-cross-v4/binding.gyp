{
  "targets": [
    {
      "target_name": "db-cross-v4-native",
      "sources": [
        "addon.cpp",
        "tlv.cpp",
        "crypto.cpp",
        "xxhash.cpp",
        "lzma_decoder.cpp",
        "backup_format.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "libraries": [
        "-lssl",
        "-lcrypto",
        "-llzma",
        "-lstdc++fs"
      ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "cflags_cc": [ "-std=c++17", "-fexceptions" ],
      "conditions": [
        [
          "OS=='linux'",
          {
            "cflags_cc": [ "-std=c++17", "-fexceptions" ]
          }
        ],
        [
          "OS=='win'",
          {
            "defines": [ "_HAS_EXCEPTIONS=1" ],
            "msvs_settings": {
              "VCCLCompilerTool": { "ExceptionHandling": 1, "AdditionalOptions": [ "/std:c++17" ] }
            }
          }
        ]
      ]
    }
  ]
}
