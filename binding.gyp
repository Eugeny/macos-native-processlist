{
  "targets": [
    {
      "target_name": "native",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions", ],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "CLANG_CXX_LIBRARY": "libc++",
        "CLANG_CXX_LANGUAGE_STANDARD": "c++17",
        "MACOSX_DEPLOYMENT_TARGET": "10.7",
      },
      "sources": [
        "src/addon.cc",
        "src/worker.cc",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
      ]
    }
  ]
}
