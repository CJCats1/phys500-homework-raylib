# Analytical solution notes

These are the expressions used by the raylib viewer. The numerical values are
rounded for display; the program evaluates the functions directly.

## 1. Rotated anisotropic quadratic

\[
 f=3x^2+2xy+2y^2
\]

\[
 \nabla f=(6x+2y,\;2x+4y),\qquad
 H=\begin{bmatrix}6&2\\2&4\end{bmatrix}.
\]

The only critical point is `(0, 0, 0)`. Its Hessian eigenvalues are
`5-sqrt(5) = 2.7639` and `5+sqrt(5) = 7.2361`, so it is a local/global
minimum.

## 2. Double well

\[
 f=(x^2-1)^2+y^2
\]

\[
 \nabla f=(4x(x^2-1),\;2y),\qquad
 H=\begin{bmatrix}12x^2-4&0\\0&2\end{bmatrix}.
\]

Critical points:

- `(-1, 0, 0)`, eigenvalues `(8, 2)`: local/global minimum.
- `(1, 0, 0)`, eigenvalues `(8, 2)`: local/global minimum.
- `(0, 0, 1)`, eigenvalues `(-4, 2)`: saddle.

## 3. Lorentzian-Gaussian splatting crater thing

\[
 f=2-e^{-(x^4+y^2)}-\frac{1}{x^2+y^4+1}.
\]

Let `A = exp(-(x^4+y^2))` and `s = x^2+y^4+1`. Then

\[
 \nabla f=\left(4x^3A+2xs^{-2},\;2yA+4y^3s^{-2}\right).
\]

The only critical point is `(0, 0, 0)`. The Hessian there has eigenvalues
`(2, 2)`, so it is a local/global minimum.

## 4. Rosenbrock valley

\[
 f=(1-x)^2+100(y-x^2)^2
\]

\[
 \nabla f=\left(2(x-1)-400x(y-x^2),\;200(y-x^2)\right),
\]

\[
 H=\begin{bmatrix}
 2-400y+1200x^2&-400x\\
 -400x&200
 \end{bmatrix}.
\]

The only critical point is `(1, 1, 0)`. Its Hessian eigenvalues are about
`0.3994` and `1001.6006`, so it is a local/global minimum.

## 5. Himmelblau's function

\[
 f=(x^2+y-11)^2+(x+y^2-7)^2.
\]

With `a=x^2+y-11` and `b=x+y^2-7`,

\[
 \nabla f=(4xa+2b,\;2a+4yb),
\]

\[
 H=\begin{bmatrix}
 12x^2+4y-42&4x+4y\\
 4x+4y&4x+12y^2-26
 \end{bmatrix}.
\]

The nine numerical critical points are:

| `(x, y, f)` | Hessian eigenvalues | classification |
|---|---:|---|
| `(3.000000, 2.000000, 0)` | `(25.7157, 82.2843)` | local/global minimum |
| `(-2.805118, 3.131313, 0)` | `(64.8404, 80.5501)` | local/global minimum |
| `(-3.779310, -3.283186, 0)` | `(70.7144, 133.7856)` | local/global minimum |
| `(3.584428, -1.848127, 0)` | `(28.6907, 105.4189)` | local/global minimum |
| `(-0.270845, -0.923039, 181.6165)` | `(-45.6052, -16.0660)` | local maximum |
| `(-3.073026, -0.081353, 104.0152)` | `(-39.6515, 72.4352)` | saddle |
| `(-0.127961, -1.953715, 178.3372)` | `(-50.6102, 20.2841)` | saddle |
| `(0.086678, 2.884255, 67.7192)` | `(-31.7066, 75.5076)` | saddle |
| `(3.385154, 0.073852, 13.3119)` | `(-14.1352, 97.5479)` | saddle |

The listed points were obtained by Newton iteration on `∇f=0` from a grid of
initial guesses and verified by checking the gradient norm. The viewer uses
the same gradient-descent update required by the assignment:

\[
 (x_{t+1},y_{t+1})=(x_t,y_t)-\eta\nabla f(x_t,y_t).
\]
