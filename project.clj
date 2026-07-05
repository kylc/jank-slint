(defproject io.github.kylc/jank-slint "0.1.1"
  :description "Jank bindings for the slint GUI toolkit"
  :url "https://github.com/kylc/jank-slint"
  :license {:name "MPL 2.0"
            :url  "https://www.mozilla.org/en-US/MPL/2.0/"}
  :dependencies [[io.github.kylc/slint-sys "2026.07-2"]]
  :plugins [[org.jank-lang/lein-jank "2026.06-2"]]
  :middleware [leiningen.jank/middleware]
  :jank {:include-dirs ["src"]}
  :profiles {:base    {:jank {:target-dir         "target/debug"
                              :optimization-level 0}}
             :release {:jank {:target-dir         "target/release"
                              :optimization-level 3
                              :runtime            :static}}}
  :source-paths ["src"])
