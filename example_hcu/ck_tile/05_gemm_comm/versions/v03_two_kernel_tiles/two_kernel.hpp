#pragma once
#include "reference_operators.hpp"

// Full per-tile storage: producers never wait for consumers or reuse a slot.
// Finite producer grid and a bounded (<80 CTA) consumer grid avoid circular
// producer/consumer residency dependencies. Actual overlap is still measured.
template<bool Trace>
__global__ __launch_bounds__(P::BlockSize,1) void compute_kernel(Args a){
    __shared__ T shared[P::LdsElements];
    int total=((a.m+255)/256)*(a.n/256);
    for(int tile=blockIdx.x;tile<total;tile+=a.compute){
        compute_tile<Trace>(a,tile,shared);
        publish(a.produced+tile,a.epoch);stamp<Trace>(a,tile,2);
    }
}
template<bool Trace,int Threads>
__global__ __launch_bounds__(Threads) void communication_kernel(Args a){
    // No LDS and independent register allocation from compute_kernel.
    int total=((a.m+255)/256)*(a.n/256);
    for(int tile=blockIdx.x;tile<total;tile+=a.comm){
        wait_local(a.produced+tile,a.epoch);
        communicate_tile<Trace>(a,tile);
    }
}
