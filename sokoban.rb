GROUND_CHAR  =  ' '
TARGET_CHAR  = '.'
TARGET_PCHAR = '+'
TARGET_CCHAR = '*'
WALL_CHAR    = '#'
CRATE_CHAR   = '$'
PLAYER_CHAR  = '@'
NEWLINE_CHAR = "\n"

GROUND_T = 0
TARGET_T = 1
WALL_T = 2
VOID_T = 3

LVL_W = 20
LVL_H = 15
LVL_SIZE = (LVL_W * LVL_H)
MAX_CRATES = 31

class Level
  def initialize(filename)
    str = PSXIO.load_file(filename)

    puts str

    @def = str.split('')

    @tiles = []
    @crates_pos = []
    @player_pos = 0
    @offsets = { h: 0, v: 0 }
  end

  def init
    draw_ground = false
    w = 0
    h = 0
    k = 0
    l = 0

    @tiles = [VOID_T] * LVL_SIZE

    @def.each do |c|
      case c
      when GROUND_CHAR
        @tiles[k] = GROUND_T if (draw_ground)
      when TARGET_CHAR
        @tiles[k] = TARGET_T
      when TARGET_PCHAR
        @tiles[k] = TARGET_T
        @player_pos = k
      when TARGET_CCHAR
        @tiles[k] = TARGET_T
        @crates_pos << k
      when WALL_CHAR
        @tiles[k] = WALL_T
        draw_ground = true
      when CRATE_CHAR
        @crates_pos << k
        @tiles[k] = GROUND_T if (draw_ground)
      when PLAYER_CHAR
        @player_pos = k
        @tiles[k] = GROUND_T if (draw_ground)
      when NEWLINE_CHAR
        w = l if (l > w)
        h += 1
        draw_ground = false

        k += LVL_W - l
        l = 0
        next
      end


      l += 1
      k += 1
    end

    @offsets = { h: (LVL_W  - w) / 2, v: (LVL_H - h) / 2 }
  end

  def debug
    puts @tiles.inspect
    puts @player_pos
    puts @crates_pos.inspect
    puts @offsets.inspect
  end
end

class Foo
  def initialize(foo)
    @bar = foo
  end

  def bar(baz)
    foobar(baz) * @bar
  end
end

a = [1, 2, 3, 4, 5]

b = {
  a: 12,
  c: 13
}

puts "hello from ruby"
puts a

def from_ruby
  1337
end

l = Level.new('\LEVELS\LEVEL15.TXT;1')
l.init
l.debug

Foo.new(a.size).bar(b[:a])

