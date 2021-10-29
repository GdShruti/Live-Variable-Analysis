; ModuleID = 'testcase3.ll'

;<=====Shruti======>
source_filename = "testcase3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @testcase3(i32 %arg, i32 %arg3) #0 {
bb:
  %i = icmp sgt i32 %arg, 0
  br i1 %i, label %bb4, label %bb5

bb4:                                              ; preds = %bb
  br label %bb7

bb5:                                              ; preds = %bb
  %i6 = sub nsw i32 0, %arg
  br label %bb7

bb7:                                              ; preds = %bb5, %bb4
  %i8 = phi i32 [ %arg, %bb4 ], [ %i6, %bb5 ]
  %i9 = icmp sgt i32 %arg3, 0
  br i1 %i9, label %bb10, label %bb11

bb10:                                             ; preds = %bb7
  br label %bb13

bb11:                                             ; preds = %bb7
  %i12 = sub nsw i32 0, %arg3
  br label %bb13

bb13:                                             ; preds = %bb11, %bb10
  %i14 = phi i32 [ %arg3, %bb10 ], [ %i12, %bb11 ]
  br label %bb15

bb15:                                             ; preds = %bb23, %bb13
  %.01 = phi i32 [ %i14, %bb13 ], [ %.12, %bb23 ]
  %.0 = phi i32 [ %i8, %bb13 ], [ %.1, %bb23 ]
  %i16 = icmp ne i32 %.0, %.01
  br i1 %i16, label %bb17, label %bb24

bb17:                                             ; preds = %bb15
  %i18 = icmp sgt i32 %.0, %.01
  br i1 %i18, label %bb19, label %bb21

bb19:                                             ; preds = %bb17
  %i20 = sub nsw i32 %.0, %.01
  br label %bb23

bb21:                                             ; preds = %bb17
  %i22 = sub nsw i32 %.01, %.0
  br label %bb23

bb23:                                             ; preds = %bb21, %bb19
  %.12 = phi i32 [ %.01, %bb19 ], [ %i22, %bb21 ]
  %.1 = phi i32 [ %i20, %bb19 ], [ %.0, %bb21 ]
  br label %bb15

bb24:                                             ; preds = %bb15
  ret i32 %.0
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 12.0.0 (https://github.com/llvm/llvm-project.git cdacffe4acc083dfb1cccb6458420eed09f9d093)"}
