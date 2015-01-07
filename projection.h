#pragma once


struct GMPmat *projection(struct GMPmat *inp, int d);
struct GMPmat *H2V(struct GMPmat *inp);
struct GMPmat *V2H(struct GMPmat *inp);
struct GMPmat *reducemat(struct GMPmat *inp);