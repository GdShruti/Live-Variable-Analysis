; ModuleID = 'testcase1.ll'

;<=====Shruti======>
source_filename = "testcase1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @testcase1(i32 %arg) #0 {
bb:
  %i = icmp sgt i32 %arg, 10
  br i1 %i, label %bb2, label %bb5

bb2:                                              ; preds = %bb
  %i3 = add nsw i32 %arg, 2
  %i4 = sub nsw i32 %arg, 1
  br label %bb8

bb5:                                              ; preds = %bb
  %i6 = mul nsw i32 %arg, 2
  %i7 = add nsw i32 %arg, 1
  br label %bb8

bb8:                                              ; preds = %bb5, %bb2
  %.01 = phi i32 [ %i4, %bb2 ], [ %i7, %bb5 ]
  br label %bb9

bb9:                                              ; preds = %bb11, %bb8
  %.1 = phi i32 [ %.01, %bb8 ], [ %i12, %bb11 ]
  %.0 = phi i32 [ %arg, %bb8 ], [ %i14, %bb11 ]
  %i10 = icmp ne i32 %.0, %.1
  br i1 %i10, label %bb11, label %bb15

bb11:                                             ; preds = %bb9
  %i12 = add nsw i32 %arg, %.0
  %i13 = mul nsw i32 %arg, 2
  %i14 = add nsw i32 %.0, 1
  br label %bb9

bb15:                                             ; preds = %bb9
  ret void
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 12.0.0 (https://github.com/llvm/llvm-project.git cdacffe4acc083dfb1cccb6458420eed09f9d093)"}
