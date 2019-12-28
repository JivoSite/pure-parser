Pod::Spec.new do |s|
  s.name                  = 'PureParser'
  s.version               = '1.0.4'
  s.summary               = 'Localization tool enpowered by special markup language; written in C++ and has some wrappers.'
  s.description           = <<-DESC
PureParser is a tool that was designed to simplify the process of adopting the localizable strings if many conditions could cause those strings to look in different ways.

PureParser allows putting the basic logic into strings themselves to control the output, and still keep it human-readable.
                       DESC

  s.author                = { 'JivoSite Inc.' => 'info@jivochat.com' }
  s.homepage              = 'https://www.jivochat.com'
  s.license               = { :type => 'MIT', :file => 'LICENSE' }

  s.source                = { :git => 'https://github.com/JivoSite/pure-parser.git', :tag => s.version.to_s }
  s.source_files          = 'cpp_src/*.hpp', 'cpp_src/PureScanner.cpp', 'cpp_src/PureParser.cpp', 'c_wrapper/*.{hpp,h}', 'c_wrapper/pure_parser.cpp', 'swift_wrapper/PureParser.swift'
  s.public_header_files   = 'c_wrapper/pure_parser.h'
  s.private_header_files  = 'cpp_src/*.hpp'
  s.swift_versions        = ['4.0', '4.2', '5.0']

  s.library               = 'c++'
  s.pod_target_xcconfig   = {
    'CLANG_CXX_LANGUAGE_STANDARD' => 'c++17',
    'GCC_WARN_INHIBIT_ALL_WARNINGS' => 'YES'
  }

  s.ios.deployment_target = '8.0'
  s.osx.deployment_target = '10.9'
  s.watchos.deployment_target = '2.0'
  s.tvos.deployment_target = '9.0'
end
