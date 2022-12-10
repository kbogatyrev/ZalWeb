{
  'targets': [
    {
      'target_name': 'addon',
      'sources': [ "cpp/addon.cc", "cpp/zal-web.cc"],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")",
        "../.vs/Zal-Core/HLib",
        "../.vs/Zal-Core/MainLib"
      ],
      'libraries': [ '/usr/lib/libZalMainLib.so' ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}
