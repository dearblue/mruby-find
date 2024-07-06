internals = File.join(__dir__, "contrib/mruby-buildconf/bootstrap.rb")
using Module.new { module_eval File.read(internals), internals, 1 }

MRuby::Gem::Specification.new("mruby-find") do |s|
  s.summary = "Find module (Find.find and Find.prune methods)"
  version = File.read(File.join(__dir__, "README.md")).scan(/^\s*[\-\*] version:\s*(\d+(?:\.\w+)+)/i).flatten[-1]
  s.version = version if version
  s.license = "BSD-2-Clause"
  s.author  = "dearblue"
  s.homepage = "https://github.com/dearblue/mruby-find"

  add_dependency "mruby-catch", core: "mruby-catch"
  add_test_dependency "mruby-dir", core: "mruby-dir"
  add_test_dependency "mruby-io", core: "mruby-io"

  configuration_recipe(
    "windows",
    abort: false,
    default: true,
    #defines: "MRUBY_FIND_WIN32",
    libraries: %w(ntdll ntoskrnl ws2_32),
    code: <<~'CODE'
      #if !defined(_WIN32) && !defined(_WIN64)
      # error "NOT WINDOWS"
      #endif

      int
      main(int argc, char *argv[])
      {
        return 0;
      }
    CODE
  )
end
