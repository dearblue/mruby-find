module Find
  def find(*paths, ignore_error: true, &report)
    # mruby-3.3.0 はキーワード引数として引き渡さないため to_enum の代わりに使う
    return ::Enumerator.new(self, :find, *paths, ignore_error: ignore_error) unless report

    ent = []
    orig = paths.size
    paths.reverse!
    while path = paths.pop
      orig = paths.size if first = (orig > paths.size)

      traverse = false
      catch(:prune) do
        traverse1 = __find_internals__(:directory?, path, (first ? false : ignore_error))
        report.call(path.dup)
        traverse = true if traverse1
      end
      next unless traverse

      path = __find_internals__(:children, path, ent, ignore_error) or next
      ent.sort!
      ent.reverse!
      ent.map! { |e| path + e }
      paths.concat ent
    end
  end

  def prune
    throw :prune
  end

  module_function :find, :prune
end
