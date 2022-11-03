# Cross Compiling configuration for the Sony PlayStation
#
# Tested only on GNU/Linux
#
MRuby::CrossBuild.new("playstation") do |conf|
  toolchain :gcc

  PSYQ_PATH = ENV['PSYQ_PATH']

  # C compiler
  conf.cc do |cc|
    cc.command = "mipsel-none-elf-gcc"
    cc.include_paths << ["#{PSYQ_PATH}/include"]
    cc.flags << %w(-g -O2 -G0 -ffreestanding -nostdlib -mno-unaligned-access)
    cc.compile_options = %(%{flags} -o "%{outfile}" -c "%{infile}")
    cc.defines << "MRB_NO_FLOAT"
    cc.defines << "MRB_NO_STDIO"
  end

  # C++ compiler
  conf.cxx do |cxx|
    cxx.command = "mipsel-none-elf-g++"
    cxx.include_paths = conf.cc.include_paths.dup
    cxx.flags = conf.cc.flags.dup
    cxx.flags << [
      "-fno-exceptions",
      "-fno-rtti",
    ]
    cxx.defines = conf.cc.defines.dup
    cxx.compile_options = conf.cc.compile_options.dup
  end

  conf.archiver do |archiver|
    archiver.command = "mipsel-none-elf-ar"
    archiver.archive_options = 'rcs "%{outfile}" %{objs}'
  end

  # No executables
  conf.bins = []

  # Do not build executable test
  conf.build_mrbtest_lib_only

  # Disable C++ exception
  conf.disable_cxx_exception

  conf.gem core: "mruby-string-ext"
  conf.gem core: "mruby-enum-ext"
end
