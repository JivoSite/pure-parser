Pod::Spec.new do |spec|
  spec.name                  = 'PureParser'
  spec.version               = '2.0.0'
  spec.summary               = 'Localization tool enpowered by special markup language; written in C++ and has some wrappers.'
  spec.description           = <<-DESC
PureParser is a tool that was designed to simplify the process of adopting the localizable strings if many conditions could cause those strings to look in different ways.

PureParser allows putting the basic logic into strings themselves to control the output, and still keep it human-readable.
                       DESC

  spec.author                = { 'JivoSite Inc.' => 'info@jivochat.com' }
  spec.homepage              = 'https://www.jivochat.com'
  spec.license               = { :type => 'MIT', :file => 'LICENSE' }

  spec.source                = { :git => 'https://github.com/JivoSite/pure-parser.git', :tag => "v#{spec.version}" }
  spec.source_files          = 'cpp_src/*.hpp', 'cpp_src/PureScanner.cpp', 'cpp_src/PureParser.cpp', 'c_wrapper/*.{hpp,h}', 'c_wrapper/pure_parser.cpp', 'swift_wrapper/PureParser.swift'
  spec.exclude_files          = [ "Package.swift" ]
  spec.public_header_files   = 'c_wrapper/pure_parser.h'
  spec.private_header_files  = 'cpp_src/*.hpp'
  spec.swift_versions        = ['4.0', '4.2', '5.0']

  spec.library               = 'c++'
  spec.pod_target_xcconfig   = {
    'CLANG_CXX_LANGUAGE_STANDARD' => 'c++17',
    'GCC_WARN_INHIBIT_ALL_WARNINGS' => 'YES'
  }

  spec.ios.deployment_target = '11.0'
  spec.osx.deployment_target = '11.0'
end
