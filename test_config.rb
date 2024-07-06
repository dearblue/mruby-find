#!ruby

require "yaml"

config = YAML.load <<~'YAML'
  common:
    gems:
    - :core: "mruby-sprintf"
    - :core: "mruby-print"
    - :core: "mruby-bin-mrbc"
    - :core: "mruby-bin-mirb"
    - :core: "mruby-bin-mruby"
    - :core: "mruby-enumerator"
  builds:
    host:
      defines: MRB_WORD_BOXING
      gems:
      - :core: "mruby-io"
      #incdirs: /usr/local/include
      #libdirs: /usr/local/lib
      #libs: z
    host32-nan:
      defines: [MRB_INT32, MRB_NAN_BOXING]
    host64++:
      defines: [MRB_INT64, MRB_NO_BOXING]
      c++abi: true
      cflags: [-std=c++11]
YAML

MRuby::Lockfile.disable rescue nil

config["builds"].each_pair do |n, c|
  MRuby::Build.new(n) do |conf|
    toolchain :clang

    conf.build_dir = File.join("build", c["build_dir"] || name)

    enable_debug
    enable_test
    case
    when c["c++abi"]
      enable_cxx_abi
    when c["c++exc"]
      enable_cxx_exception
    end

    compilers.each do |cc|
      cc.defines << [*c["defines"]]
      cc.flags << [*c["cflags"]]
      cc.include_paths << [*c["incdirs"]]
    end

    linker.library_paths << [*c["libdirs"]]
    linker.libraries << [*c["libs"]]

    Array(config.dig("common", "gems")).each { |*g| gem *g }
    Array(c["gems"]).each { |*g| gem *g }

    gem __dir__ do |g|
      if g.cc.command =~ /\b(?:g?cc|clang)\d*\b/
        g.cc.flags << "-std=c11" unless c["c++abi"]
        g.cc.flags << "-pedantic"
        g.cc.flags << "-Wall"
      end
    end
  end
end
