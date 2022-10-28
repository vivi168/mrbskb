# Cross Compiling configuration for the Sony PlayStation
#
# Tested only on GNU/Linux
#
MRuby::CrossBuild.new("playstation") do |conf|
  toolchain :gcc

  PSYQ_PATH = "/home/vbihl/PSYQSDK"

  # C compiler
  conf.cc do |cc|
    cc.command = "mipsel-none-elf-gcc"
    cc.include_paths << ["#{PSYQ_PATH}/include"]
    cc.flags << %w( -g -G0 -ffreestanding -nostdlib)
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

  # Linker
  conf.linker do |linker|
    linker.command = "mipsel-none-elf-gcc"
    linker.flags << %w(-g -ffreestanding -nostdlib)
    linker.libraries << [
      "libcard.a",
      "libpress.a",
      "libgpu.a",
      "libgs.a",
      "libgte.a",
      "libcd.a",
      "libetc.a",
      "libsn.a",
      "libsnd.a",
      "libspu.a",
      "libmath.a",
      "libcomb.a",
      "libcard.a",
      "libtap.a",
      "libsio.a",
      "libpad.a",
      "libc2.a",
      "libapi.a",
      "extra.a"
    ]
    linker.flags_after_libraries << '-lgcc'
    linker.library_paths << ["#{PSYQ_PATH}/lib"]
  end

  # No executables
  conf.bins = []

  # Do not build executable test
  conf.build_mrbtest_lib_only

  # Disable C++ exception
  conf.disable_cxx_exception

  conf.gem core: "mruby-compiler"
end
