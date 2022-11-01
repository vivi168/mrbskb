DIR_UP = 0
DIR_LEFT = 1
DIR_DOWN = 2
DIR_RIGHT = 3

GROUND_T = 0
TARGET_T = 1
WALL_T = 2
VOID_T = 3

class Level
  GROUND_CHAR  = ' '
  TARGET_CHAR  = '.'
  TARGET_PCHAR = '+'
  TARGET_CCHAR = '*'
  WALL_CHAR    = '#'
  CRATE_CHAR   = '$'
  PLAYER_CHAR  = '@'
  NEWLINE_CHAR = "\n"

  def initialize(index)
    @index = index
    @str = PSX::IO.load_file(filename)
    @loaded = !@str.nil?

    return unless @loaded

    puts @str

    definition = @str.split('')
    @tiles = [VOID_T] * PSX::Graph::LVL_SIZE
    @crates_pos = []
    @player_pos = 0
    @offsets = { h: 0, v: 0 }
    @target_steps = 0
    @current_steps = 0

    draw_ground = false
    w = 0
    h = 0
    k = 0
    l = 0

    definition.each do |c|
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

        k += PSX::Graph::LVL_W - l
        l = 0
        next
      end

      l += 1
      k += 1
    end

    @offsets = { h: (PSX::Graph::LVL_W  - w) / 2, v: (PSX::Graph::LVL_H - h) / 2 }

    true
  end

  def move_player(direction)
    prev_pos = @player_pos

    case direction
    when DIR_UP
      @player_pos -= PSX::Graph::LVL_W
    when DIR_DOWN
      @player_pos += PSX::Graph::LVL_W
    when DIR_LEFT
      @player_pos -= 1
    when DIR_RIGHT
      @player_pos += 1
    end

    crate_pos = @crates_pos.detect { |c| c == @player_pos }
    @player_pos = prev_pos unless move_crate(crate_pos, direction)
    @player_pos = prev_pos if @tiles[@player_pos ] == WALL_T
    @player_pos = prev_pos if @player_pos < 0 || @player_pos > PSX::Graph::LVL_SIZE

    moved = @player_pos != prev_pos
    @current_steps += 1 if moved

    moved
  end

  def move_crate(crate_pos, direction)
    return true if crate_pos.nil? # allow player movement if pos is nil

    prev_pos = crate_pos

    case direction
    when DIR_UP
      crate_pos -= PSX::Graph::LVL_W
    when DIR_DOWN
      crate_pos += PSX::Graph::LVL_W
    when DIR_LEFT
      crate_pos -= 1
    when DIR_RIGHT
      crate_pos += 1
    end

    crate_pos = prev_pos if @crates_pos.detect { |c| c == crate_pos }
    crate_pos = prev_pos if @tiles[crate_pos] == WALL_T
    crate_pos = prev_pos if crate_pos < 0 || crate_pos > PSX::Graph::LVL_SIZE

    i = @crates_pos.index(prev_pos)
    @crates_pos[i] = crate_pos

    crate_pos != prev_pos
  end

  def done?
    @crates_pos.map { |c| @tiles[c] == TARGET_T }.all?(true)
  end

  def render
    PSX::Graph.clear_otag
    draw_tile(PSX::Graph::PLAYER_IDX, @player_pos)

    @crates_pos.each do |crate_pos|
      ttype = @tiles[crate_pos] == TARGET_T ? PSX::Graph::CRATET_IDX : PSX::Graph::CRATEG_IDX
      draw_tile(ttype, crate_pos)
    end

    @tiles.each_with_index do |tile, pos|
      case tile
      when VOID_T
        next
      when GROUND_T
        draw_tile(PSX::Graph::GROUND_IDX, pos)
      when TARGET_T
        draw_tile(PSX::Graph::TARGET_IDX, pos)
      when WALL_T
        draw_tile(PSX::Graph::WALL_IDX, pos)
      end
    end

    PSX::Graph.draw_tpage
    PSX::Graph.print("LEVEL #{@index + 1} - STEP #{@current_steps}")

    PSX::Graph.delay
  end

  def draw_tile(ttype, pos)
    PSX::Graph.draw_tile(@offsets[:h], @offsets[:v], ttype, pos)
  end

  def filename
    idx = (@index + 1).to_s.rjust(2, '0')

    "\\LEVELS\\LEVEL#{idx}.TXT;1"
  end

  def debug
    if !@loaded
      puts 'level is empty !'
      return
    end
    puts "level size: #{@tiles.size}"
    puts @tiles.inspect
    puts "player pos #{@player_pos}, crate count #{@crates_pos.size}"
    puts @crates_pos.inspect
    puts @offsets.inspect
  end
end

class Sokoban
  def initialize
    @current_level = 0

    @level = Level.new(@current_level)
  end

  def mainloop
    loop {
      PSX::Pad.poll

      if PSX::Pad.pressed?(PSX::Pad::KEY_UP)
        @level.move_player(DIR_UP)
      elsif PSX::Pad.pressed?(PSX::Pad::KEY_DOWN)
        @level.move_player(DIR_DOWN)
      elsif PSX::Pad.pressed?(PSX::Pad::KEY_LEFT)
        @level.move_player(DIR_LEFT)
      elsif PSX::Pad.pressed?(PSX::Pad::KEY_RIGHT)
        @level.move_player(DIR_RIGHT)
      end

      @level = Level.new(@current_level) if PSX::Pad.pressed?(PSX::Pad::KEY_TRIANGLE)

      if @level.done? # => next level
        puts "well done!"
        @current_level += 1
        @level = Level.new(@current_level)
      end

      @level.render
    }
  end
end

def from_ruby
  1337
end

Sokoban.new.mainloop
