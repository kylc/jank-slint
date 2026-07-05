(defproject io.github.kylc/slint-sys "2026.07-1"
  :description "Raw package for slint."
  :url "https://github.com/kylc/slint-sys"
  :license {:name "MPL 2.0"
            :url  "https://www.mozilla.org/en-US/MPL/2.0/"}
  :build-dependencies [[org.jank-lang.commons/jank-build-cmake "2026.06-6"]]
  :dependencies []
  :plugins [[org.jank-lang/lein-jank "2026.06-2"]]
  :middleware [leiningen.jank/middleware]
  :verbatim-paths ["lib/slint" "lib/corrision" "CMakeLists.txt"])
