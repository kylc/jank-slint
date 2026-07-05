(require '[babashka.fs :as fs]
         '[jank.build.cmake :as cmake])

(let [src-dir (:src-dir *input*)
      out-dir (:out-dir *input*)]
  (cmake/build (update *input* :src-dir #(fs/path % "lib/slint"))
               {"FETCHCONTENT_SOURCE_DIR_CORROSION" (fs/path src-dir "lib/corrosion")
                "SLINT_FEATURE_TESTING"             "OFF"})
  
  (println (str "jank-build::include-dir=" (fs/path out-dir "include" "slint")))
  (println (str "jank-build::link-dir=" (fs/path out-dir "lib")))
  (println (str "jank-build::link-library=" "slint_cpp")))
