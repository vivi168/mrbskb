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

Foo.new(a.size).bar(b[:a])
