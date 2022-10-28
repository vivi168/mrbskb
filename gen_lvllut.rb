#!/usr/bin/env ruby

def filenames(files, fmt: :pc)
  files.each do |file|
    if fmt == :pc
      puts "    \"#{file}\","
    else
      filepath = file.gsub('/') { "\\\\" }.upcase
      puts "    \"\\\\#{filepath};1\","
    end
  end
end

path = File.expand_path(File.dirname(__FILE__))
files = Dir.glob('levels/level*.txt', base: path).sort

if ARGV[0] == "LUT"
  puts '#include "level.h"'
  puts 'const char *level_LUT[LEVEL_COUNT] = {'
  filenames(files, fmt: :psx)
  puts '};'
else
  files.size.times do |i|
    puts "<file name=\"LEVEL#{'%02i' % (i+1)}.TXT\" type=\"data\" source=\"levels/level#{'%02i' % (i+1)}.txt\"/>"
  end
end
