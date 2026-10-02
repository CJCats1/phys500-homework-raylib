from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib.pagesizes import letter
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import inch
from reportlab.platypus import (
    Image,
    KeepTogether,
    PageBreak,
    Paragraph,
    SimpleDocTemplate,
    Spacer,
    Table,
    TableStyle,
)


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "output" / "pdf" / "PHYS500_homework_submission.pdf"

SCREENSHOTS = [
    "/tmp/codex-clipboard-98d40865-9e9f-4e3c-8fa7-b446ce0ddb7d.png",
    "/tmp/codex-clipboard-98078fca-1e30-4b66-967f-28e335237976.png",
    "/tmp/codex-clipboard-fb4a4691-a72d-4a6a-b2e9-91824965e803.png",
    "/tmp/codex-clipboard-0fb6c705-f543-41ca-87ff-acf66b445535.png",
    "/tmp/codex-clipboard-cc58f227-75e2-4959-a0d0-700021459b89.png",
]


styles = getSampleStyleSheet()
styles.add(
    ParagraphStyle(
        name="TitleCenter",
        parent=styles["Title"],
        alignment=TA_CENTER,
        fontSize=22,
        leading=28,
        textColor=colors.HexColor("#17365D"),
        spaceAfter=14,
    )
)
styles.add(
    ParagraphStyle(
        name="SubtitleCenter",
        parent=styles["Normal"],
        alignment=TA_CENTER,
        fontSize=12,
        leading=17,
        textColor=colors.HexColor("#4F5B66"),
        spaceAfter=10,
    )
)
styles.add(
    ParagraphStyle(
        name="Section",
        parent=styles["Heading1"],
        fontSize=17,
        leading=21,
        textColor=colors.HexColor("#17365D"),
        spaceBefore=0,
        spaceAfter=8,
    )
)
styles.add(
    ParagraphStyle(
        name="Subsection",
        parent=styles["Heading2"],
        fontSize=11,
        leading=14,
        textColor=colors.HexColor("#2F5597"),
        spaceBefore=7,
        spaceAfter=3,
    )
)
styles.add(
    ParagraphStyle(
        name="BodySmall",
        parent=styles["BodyText"],
        fontSize=9.2,
        leading=12.2,
        spaceAfter=5,
    )
)
styles.add(
    ParagraphStyle(
        name="Formula",
        parent=styles["Code"],
        fontName="Courier",
        fontSize=8.8,
        leading=11.2,
        leftIndent=10,
        textColor=colors.HexColor("#222222"),
        spaceAfter=5,
    )
)
styles.add(
    ParagraphStyle(
        name="Caption",
        parent=styles["Normal"],
        alignment=TA_CENTER,
        fontSize=8.5,
        leading=11,
        textColor=colors.HexColor("#555555"),
        spaceBefore=6,
    )
)
styles.add(
    ParagraphStyle(
        name="TableText",
        parent=styles["Normal"],
        fontSize=7.3,
        leading=9,
    )
)


def P(text, style="BodySmall"):
    return Paragraph(text, styles[style])


def formula(text):
    return P(text, "Formula")


def critical_table(rows):
    header = [P("Point (x, y, z)", "TableText"), P("Hessian eigenvalues", "TableText"), P("Classification", "TableText"), P("||grad f||2", "TableText")]
    data = [header]
    for row in rows:
        data.append([P(cell, "TableText") for cell in row])
    table = Table(data, colWidths=[1.75 * inch, 1.35 * inch, 1.55 * inch, 0.9 * inch], repeatRows=1)
    table.setStyle(
        TableStyle(
            [
                ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#D9EAF7")),
                ("TEXTCOLOR", (0, 0), (-1, 0), colors.HexColor("#17365D")),
                ("GRID", (0, 0), (-1, -1), 0.35, colors.HexColor("#AAB7C4")),
                ("VALIGN", (0, 0), (-1, -1), "TOP"),
                ("LEFTPADDING", (0, 0), (-1, -1), 4),
                ("RIGHTPADDING", (0, 0), (-1, -1), 4),
                ("TOPPADDING", (0, 0), (-1, -1), 3),
                ("BOTTOMPADDING", (0, 0), (-1, -1), 3),
            ]
        )
    )
    return table


def analytical_pages():
    pages = []

    pages.append(
        [
            P("PHYS 500 - Homework 1-2", "TitleCenter"),
            P("Gradient Descent Optimization and Graphics", "SubtitleCenter"),
            Spacer(1, 0.12 * inch),
            P("Analytical mathematics and raylib visualization record", "SubtitleCenter"),
            Spacer(1, 0.25 * inch),
            P("Student: ________________________________________________", "BodySmall"),
            P("Date: _________________________________________________", "BodySmall"),
            Spacer(1, 0.18 * inch),
            P("Method summary", "Section"),
            P("For each landscape f(x,y), I compute the gradient grad f and Hessian H, solve grad f = 0 for critical points, and classify each point using the eigenvalues of H. Numerical critical points for Himmelblau's function were found with Newton iteration from a grid of initial guesses. The displayed coordinates are rounded; the unrounded Newton solutions were checked using the gradient norm.", "BodySmall"),
            formula("Gradient descent update:   (x_(t+1), y_(t+1)) = (x_t, y_t) - eta grad f(x_t, y_t)"),
            P("Classification rule: positive eigenvalues indicate a local minimum, negative eigenvalues indicate a local maximum, mixed signs indicate a saddle, and a zero eigenvalue makes the second-order test inconclusive.", "BodySmall"),
            Spacer(1, 0.12 * inch),
            P("Visualization key", "Section"),
            P("The raylib screenshots use the following visual layers: the colored translucent surface is f(x,y); contour curves are drawn on the xy plane; blue arrows show the gradient field; colored markers show critical points; and the yellow path shows gradient descent. Green markers are minima, red marks a maximum, magenta marks saddles, white is the current descent point, and yellow is the initial point.", "BodySmall"),
            Spacer(1, 0.18 * inch),
            P("The following pages present the analytical solution for all five landscapes followed by the corresponding labeled raylib screenshot.", "BodySmall"),
        ]
    )

    pages.append(
        [
            P("Landscape 1 - Rotated anisotropic quadratic", "Section"),
            formula("f(x,y) = 3x^2 + 2xy + 2y^2"),
            P("I. Gradient", "Subsection"),
            formula("grad f = [ 6x + 2y,  2x + 4y ]^T"),
            P("II. Hessian", "Subsection"),
            formula("H = [[6, 2], [2, 4]]"),
            P("III-IV. Critical point and classification", "Subsection"),
            formula("grad f = 0  gives  x = 0, y = 0, z = f(0,0) = 0.") ,
            critical_table([
                ["(0, 0, 0)", "5 - sqrt(5) = 2.7639; 5 + sqrt(5) = 7.2361", "local/global minimum", "0"],
            ]),
            Spacer(1, 0.08 * inch),
            P("Both eigenvalues are positive, so the critical point is a strict local minimum. Because this quadratic form is positive definite, it is also the global minimum.", "BodySmall"),
        ]
    )

    pages.append(
        [
            P("Landscape 2 - Double well", "Section"),
            formula("f(x,y) = (x^2 - 1)^2 + y^2"),
            P("I. Gradient", "Subsection"),
            formula("grad f = [ 4x(x^2 - 1),  2y ]^T"),
            P("II. Hessian", "Subsection"),
            formula("H = [[12x^2 - 4, 0], [0, 2]]"),
            P("III-IV. Critical points and classification", "Subsection"),
            formula("grad f = 0  gives  y = 0 and x in {-1, 0, 1}.") ,
            critical_table([
                ["(-1, 0, 0)", "8, 2", "local/global minimum", "0"],
                ["(1, 0, 0)", "8, 2", "local/global minimum", "0"],
                ["(0, 0, 1)", "-4, 2", "saddle", "0"],
            ]),
            Spacer(1, 0.08 * inch),
            P("At x = +/-1 both eigenvalues are positive. At the origin the eigenvalues have opposite signs, so the origin is a saddle point.", "BodySmall"),
        ]
    )

    pages.append(
        [
            P("Landscape 3 - Lorentzian-Gaussian splatting crater", "Section"),
            formula("f(x,y) = 2 - exp(-(x^4 + y^2)) - 1/(x^2 + y^4 + 1)"),
            P("I. Gradient", "Subsection"),
            formula("Let A = exp(-(x^4+y^2)), s = x^2+y^4+1.") ,
            formula("grad f = [ 4x^3 A + 2x s^(-2),  2y A + 4y^3 s^(-2) ]^T"),
            P("II. Hessian", "Subsection"),
            formula("Hxx = 12x^2 A - 16x^6 A + 2s^(-2) - 8x^2 s^(-3)"),
            formula("Hxy = Hyx = -8x^3 y A - 16x y^3 s^(-3)"),
            formula("Hyy = 2A - 4y^2 A + 12y^2 s^(-2) - 32y^6 s^(-3)"),
            P("III-IV. Critical point and classification", "Subsection"),
            formula("Both gradient components have factors x and y with positive remaining factors, so the only critical point is (0,0,0)."),
            critical_table([
                ["(0, 0, 0)", "2, 2", "local/global minimum", "0"],
            ]),
            Spacer(1, 0.08 * inch),
            P("Since H(0,0) = [[2,0],[0,2]], the Hessian is positive definite and the second-order test gives a strict local minimum.", "BodySmall"),
        ]
    )

    pages.append(
        [
            P("Landscape 4 - Rosenbrock valley", "Section"),
            formula("f(x,y) = (1-x)^2 + 100(y-x^2)^2"),
            P("I. Gradient", "Subsection"),
            formula("grad f = [ 2(x-1) - 400x(y-x^2),  200(y-x^2) ]^T"),
            P("II. Hessian", "Subsection"),
            formula("H = [[2 - 400y + 1200x^2, -400x], [-400x, 200]]"),
            P("III-IV. Critical point and classification", "Subsection"),
            formula("The second component gives y = x^2. Substitution into the first gives x = 1, so the point is (1,1,0)."),
            critical_table([
                ["(1, 1, 0)", "0.3994, 1001.6006", "local/global minimum", "0"],
            ]),
            Spacer(1, 0.08 * inch),
            P("Both eigenvalues are positive, so (1,1) is a strict local minimum. Since f is a sum of squares and reaches zero there, it is also the global minimum. The narrow valley explains why gradient descent needs many iterations.", "BodySmall"),
        ]
    )

    himmelblau_rows = [
        ["(3.000000, 2.000000, 0)", "25.7157, 82.2843", "local/global minimum", "0"],
        ["(-2.805118, 3.131313, 0)", "64.8404, 80.5501", "local/global minimum", "3.9e-5"],
        ["(-3.779310, -3.283186, 0)", "70.7144, 133.7856", "local/global minimum", "3.1e-5"],
        ["(3.584428, -1.848127, 0)", "28.6907, 105.4189", "local/global minimum", "4.2e-5"],
        ["(-0.270845, -0.923039, 181.6165)", "-45.6052, -16.0660", "local maximum", "2.3e-5"],
        ["(-3.073026, -0.081353, 104.0152)", "-39.6515, 72.4352", "saddle", "1.8e-5"],
        ["(-0.127961, -1.953715, 178.3372)", "-50.6102, 20.2841", "saddle", "1.7e-5"],
        ["(0.086678, 2.884255, 67.7192)", "-31.7066, 75.5076", "saddle", "3.0e-5"],
        ["(3.385154, 0.073852, 13.3119)", "-14.1352, 97.5479", "saddle", "1.6e-5"],
    ]
    pages.append(
        [
            P("Landscape 5 - Himmelblau's function", "Section"),
            formula("f(x,y) = (x^2 + y - 11)^2 + (x + y^2 - 7)^2"),
            P("I. Gradient", "Subsection"),
            formula("Let a = x^2+y-11 and b = x+y^2-7."),
            formula("grad f = [ 4xa + 2b,  2a + 4yb ]^T"),
            P("II. Hessian", "Subsection"),
            formula("H = [[12x^2+4y-42, 4x+4y], [4x+4y, 4x+12y^2-26]]"),
            P("III-IV. Numerical critical points and classification", "Subsection"),
            P("Newton iteration was run from a grid of initial guesses. The table shows rounded coordinates; the reported gradient norms are evaluated at those rounded coordinates. The unrounded solver results were below 1e-8.", "BodySmall"),
            critical_table(himmelblau_rows),
        ]
    )

    return pages


def figure_page(image_path, landscape_number, title):
    image = Image(image_path)
    max_width = 7.35 * inch
    max_height = 5.75 * inch
    scale = min(max_width / image.imageWidth, max_height / image.imageHeight)
    image.drawWidth = image.imageWidth * scale
    image.drawHeight = image.imageHeight * scale
    return [
        P(f"Figure {landscape_number} - {title}", "Section"),
        P("Raylib visualization screenshot. The in-application panel identifies the landscape and shows the active visual layers, critical points, and gradient-descent state.", "BodySmall"),
        Spacer(1, 0.1 * inch),
        image,
        P(f"Figure {landscape_number}. {title}.", "Caption"),
    ]


def on_page(canvas, doc):
    canvas.saveState()
    width, height = letter
    canvas.setStrokeColor(colors.HexColor("#D9E2F3"))
    canvas.setLineWidth(0.5)
    canvas.line(doc.leftMargin, height - 0.45 * inch, width - doc.rightMargin, height - 0.45 * inch)
    canvas.setFont("Helvetica", 8)
    canvas.setFillColor(colors.HexColor("#667085"))
    canvas.drawString(doc.leftMargin, 0.35 * inch, "PHYS 500 - Homework 1-2")
    canvas.drawRightString(width - doc.rightMargin, 0.35 * inch, f"Page {doc.page}")
    canvas.restoreState()


def build():
    for path in SCREENSHOTS:
        if not Path(path).exists():
            raise FileNotFoundError(path)

    story = []
    analytic = analytical_pages()
    titles = [
        "Rotated anisotropic quadratic",
        "Double well",
        "Gaussian/Lorentzian crater",
        "Rosenbrock valley",
        "Himmelblau's function",
    ]

    story.extend(analytic[0])
    story.append(PageBreak())
    for index in range(5):
        story.extend(analytic[index + 1])
        story.append(PageBreak())
        story.extend(figure_page(SCREENSHOTS[index], index + 1, titles[index]))
        if index != 4:
            story.append(PageBreak())

    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    doc = SimpleDocTemplate(
        str(OUTPUT),
        pagesize=letter,
        rightMargin=0.55 * inch,
        leftMargin=0.55 * inch,
        topMargin=0.65 * inch,
        bottomMargin=0.55 * inch,
        title="PHYS 500 Homework 1-2 Submission",
        author="",
    )
    doc.build(story, onFirstPage=on_page, onLaterPages=on_page)
    print(OUTPUT)


if __name__ == "__main__":
    build()

