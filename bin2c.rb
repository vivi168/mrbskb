def hex(num, rjust_len = 2)
  value = (num || 0).to_s(16).rjust(rjust_len, '0').downcase

  "0x#{value}"
end

filename = ARGV[0]

raise "usage: bin2c filename name" if filename.nil?
raise "File #{filename} not found" unless File.file?(filename)

size = File.size(filename)
var_name = ARGV[1]
var_name = "sokoban_rb" if var_name.nil?
guard_var = "__#{var_name.upcase}_H"
len_var = "#{var_name.upcase}_LEN"

bytearray = IO.binread(filename).unpack('C' * size).map { |b| hex(b) }.each_slice(12)

header_file = ""
source_file = ""
header_filename = "#{var_name}.h"
source_filename = "#{var_name}.c"

# header file

header_file << "#ifndef #{guard_var}\n"
header_file << "#define #{guard_var}\n\n"

header_file << "#define #{len_var} #{size}\n\n"
header_file << "extern const unsigned char #{var_name}[#{len_var}];\n\n"

header_file << "#endif\n"

# source file

source_file << "#include \"#{header_filename}\"\n\n"

source_file << "const unsigned char #{var_name}[#{len_var}] = {\n"
bytearray.each_with_index do |ba, i|
  c = i == bytearray.count - 1 ? [] : ["\n"]
  source_file << " " * 4 << (ba + c).join(', ')
end
source_file << "\n};\n"

File.open(header_filename, 'w') { |file| file.write(header_file) }
File.open(source_filename, 'w') { |file| file.write(source_file) }
