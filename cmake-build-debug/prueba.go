"hola\c"

func sum(s []float32, c chan float32) {
        var sum float32 = 0b0x3, _42, 42_, 4__2, 0_xBadFace, 0x.p1, 1p-2, 0x1.5e-2
        var suma float32 = 1_.5, 1._5, 1.5_e1, 1.5e_1, 1.5e1_
        for _, v := range s {
                sum += v
        }
        c <- sum // send sum to c
}
