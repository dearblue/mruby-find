mruby-find - traverse a file hierarchy
========================================================================


ちゅういとせいげん
------------------------------------------------------------------------

  - Windows で `MRB_UTF8_STRING` が定義されない場合、`setlocale(LC_ALL, "")` のような呼び出しが必要になります。
  - Windows で `MRB_UTF8_STRING` が定義された場合、内部ではワイド文字として処理します。
    ただし mruby-3.3 の時点では、まだ mruby-dir や mruby-io、mruby-print などがワイド文字として処理しないため、パスの相互の受け渡しで問題が発生します。
  - Windows への対応は、おそらく Windows 2000 以降の NT 系列が必要です。
    MS-DOS 系の Windows には対応していません。


できること
------------------------------------------------------------------------

  - module `Find`
      - `Find.find(*paths, ignore_error: true)                 ->  an Enumerator`
      - `Find.find(*paths, ignore_error: true) { |path| ... }  ->  nil`
      - `Find.prune`


くみこみかた
------------------------------------------------------------------------

`build_config.rb` に gem として追加して、mruby をビルドして下さい。

```ruby
MRuby::Build.new do |conf|
  conf.gem "mruby-find", github: "dearblue/mruby-find"
end
```

- - - -

mruby gem パッケージとして依存したい場合、`mrbgem.rake` に記述して下さい。

```ruby
# mrbgem.rake
MRuby::Gem::Specification.new("mruby-XXX") do |spec|
  ...
  spec.add_dependency "mruby-find", github: "dearblue/mruby-find"
end
```


つかいかた
------------------------------------------------------------------------

Ruby の `find` ライブラリを模倣しています。
全く同じであることを保証するわけではありませんが、同じ挙動となることを期待しています。


Specification
------------------------------------------------------------------------

  - Package name: mruby-find
  - Version: 0.1
  - Project status: PROTOTYPE
  - Author: [dearblue](https://github.com/dearblue)
  - Project page: <https://github.com/dearblue/mruby-find>
  - Licensing: [2 clause BSD License](LICENSE)
  - Dependency external mrbgems: (NONE)
  - Bundled C libraries (git-submodules): (NONE)
  - Dependency external C libraries: (NONE)
