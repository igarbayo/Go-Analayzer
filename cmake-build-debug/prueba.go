"hola\c"

func sum(s []float32, c chan float32) {
        var sum float32 = 1.5e1_
        for _, v := range s {
                sum += v
        }
        c <- sum // send sum to c
}
