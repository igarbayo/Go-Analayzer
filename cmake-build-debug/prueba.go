"hola\c"

func sum(s []float32, c chan float32) {
        var sum float32 = 0_
        for _, v := range s {
                sum += v
        }
        c <- sum // send sum to c
}
