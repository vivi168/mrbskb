class Level
  GROUND_CHAR  = ' '
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

  DIR_UP = 0
  DIR_LEFT = 1
  DIR_DOWN = 2
  DIR_RIGHT = 3

  LVL_W = 20
  LVL_H = 15
  LVL_SIZE = (LVL_W * LVL_H)
  MAX_CRATES = 31

  def initialize(index)
    @str = PSX::IO.load_file(filename(index))
    @loaded = !@str.nil?

    return unless @loaded

    puts @str

    definition = @str.split('')
    @tiles = [VOID_T] * LVL_SIZE
    @crates_pos = []
    @player_pos = 0
    @offsets = { h: 0, v: 0 }

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

        k += LVL_W - l
        l = 0
        next
      end

      l += 1
      k += 1
    end

    @offsets = { h: (LVL_W  - w) / 2, v: (LVL_H - h) / 2 }

    true
  end

  def move_player(direction)
    prev_pos = @player_pos
    # moved = false

    case direction
    when DIR_UP
      @player_pos -= LVL_W
    when DIR_DOWN
      @player_pos += LVL_W
    when DIR_LEFT
      @player_pos -= 1
    when DIR_RIGHT
      @player_pos += 1
    end

    # TODO: replace with detect
    # @crates_pos.each do |crate_pos|
    #   if @player_pos == crate_pos
    #     moved = move_crate(crate_pos, direction)

    #     @player_pos = prev_pos
    #     break
    #   end
    # end
    crate_pos = @crates_pos.detect { |c| c == @player_pos }
    @player_pos = prev_pos unless move_crate(crate_pos, direction)
    @player_pos = prev_pos if @tiles[@player_pos ] == WALL_T
    @player_pos = prev_pos if @player_pos < 0 || @player_pos > LVL_SIZE
    puts "moving player... #{@player_pos}"

    @player_pos != prev_pos
  end

  # return true if crates has moved or if crate_pos is nil
  def move_crate(crate_pos, direction)
    return true if crate_pos.nil? # allow player movement if pos is nil

    prev_pos = crate_pos

    case direction
    when DIR_UP
      crate_pos -= LVL_W
    when DIR_DOWN
      crate_pos += LVL_W
    when DIR_LEFT
      crate_pos -= 1
    when DIR_RIGHT
      crate_pos += 1
    end

    # TODO: can use index instead of detect
    crate_pos = prev_pos if @crates_pos.detect { |c| c == crate_pos }
    crate_pos = prev_pos if @tiles[crate_pos] == WALL_T
    crate_pos = prev_pos if crate_pos < 0 || crate_pos > LVL_SIZE

    i = @crates_pos.index(prev_pos)
    @crates_pos[i] = crate_pos

    puts "moving crates... #{@crates_pos.inspect}"

    crate_pos != prev_pos
  end

  def done?
    @crates_pos.map { |c| @tiles[c] == TARGET_T }.all?(true)
  end

  def filename(index)
    idx = (index + 1).to_s.rjust(2, '0')

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

    l = Level.new(@current_level)
    l.debug
    puts l.done?
  end

  def mainloop
    loop {

      PSX::Pad.poll

      if PSX::Pad.pressed?(PSX::Pad::KEY_UP)
        puts 'key up'
      elsif PSX::Pad.pressed?(PSX::Pad::KEY_DOWN)
        puts 'key down'
      elsif PSX::Pad.pressed?(PSX::Pad::KEY_LEFT)
        puts 'key left'
      elsif PSX::Pad.pressed?(PSX::Pad::KEY_RIGHT)
        puts 'key right'
      end

    }
  end
end

def from_ruby
  1337
end

Sokoban.new #.mainloop

# puts "down..."
# l.move_player(DIR_DOWN)
# puts "up..."
# l.move_player(DIR_UP)
# puts "left..."
# l.move_player(DIR_LEFT)
# puts "right..."
# l.move_player(DIR_RIGHT)
