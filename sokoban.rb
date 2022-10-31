class Level
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

s = PSXIO.load_file('\LEVELS\LEVEL01.TXT;1')

puts s

Foo.new(a.size).bar(b[:a])

