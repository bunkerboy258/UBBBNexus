#include "SBBBCharacterCustomizationIcon.h"
#include "Rendering/DrawElementTypes.h"

void SBBBCharacterCustomizationIcon::Construct(const FArguments& InArgs)
{
    Symbol = InArgs._Symbol;
    Size = InArgs._Size;
    Color = InArgs._Color;
}

FVector2D SBBBCharacterCustomizationIcon::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return FVector2D(Size);
}

int32 SBBBCharacterCustomizationIcon::OnPaint(const FPaintArgs& Args, const FGeometry& Geometry,
    const FSlateRect& CullingRect, FSlateWindowElementList& Elements, int32 Layer,
    const FWidgetStyle& Style, bool bParentEnabled) const
{
    const FVector2D Available = Geometry.GetLocalSize();
    const double Extent = FMath::Min(static_cast<double>(Size), FMath::Min(Available.X, Available.Y));
    const double Scale = Extent / 24.0;
    const FVector2D Origin = (Available - FVector2D(Extent)) * 0.5;
    const FLinearColor Tint = Color.Get() * Style.GetColorAndOpacityTint();
    const auto Stroke = [&](std::initializer_list<FVector2D> Vertices)
    {
        TArray<FVector2D> Points;
        for (const FVector2D& Vertex : Vertices)
        {
            Points.Add(Origin + Vertex * Scale);
        }
        FSlateDrawElement::MakeLines(Elements, Layer, Geometry.ToPaintGeometry(), Points,
            ESlateDrawEffect::None, Tint, true, 1.8f);
    };
    if (Symbol == TEXT("Head"))
    {
        Stroke({{8, 4}, {16, 4}, {18, 8}, {17, 14}, {14, 17}, {10, 17}, {7, 14}, {6, 8}, {8, 4}});
        Stroke({{9, 17}, {9, 19}, {5, 21}, {19, 21}, {15, 19}, {15, 17}});
    }
    if (Symbol == TEXT("Helmet"))
    {
        Stroke({{3, 14}, {4, 8}, {8, 4}, {16, 4}, {20, 8}, {21, 14}, {3, 14}});
        Stroke({{4, 14}, {6, 18}, {8, 18}, {8, 14}});
        Stroke({{16, 14}, {16, 18}, {18, 18}, {20, 14}});
        Stroke({{8, 18}, {10, 21}, {14, 21}, {16, 18}});
    }
    if (Symbol == TEXT("Body"))
    {
        Stroke({{8, 4}, {5, 5}, {2, 11}, {6, 13}, {7, 10}, {7, 21}, {17, 21}, {17, 10}, {18, 13}, {22, 11}, {19, 5}, {16, 4}, {14, 7}, {10, 7}, {8, 4}});
        Stroke({{12, 7}, {12, 20}});
    }
    if (Symbol == TEXT("Vest"))
    {
        Stroke({{6, 3}, {9, 3}, {10, 7}, {14, 7}, {15, 3}, {18, 3}, {18, 9}, {21, 12}, {20, 21}, {4, 21}, {3, 12}, {6, 9}, {6, 3}});
        Stroke({{7, 12}, {17, 12}, {17, 18}, {7, 18}, {7, 12}});
        Stroke({{10, 12}, {10, 18}});
        Stroke({{14, 12}, {14, 18}});
    }
    if (Symbol == TEXT("Arms"))
    {
        Stroke({{7, 3}, {4, 12}, {3, 20}, {8, 21}, {10, 13}, {10, 5}});
        Stroke({{17, 3}, {20, 12}, {21, 20}, {16, 21}, {14, 13}, {14, 5}});
        Stroke({{4, 16}, {8, 17}});
        Stroke({{16, 17}, {20, 16}});
    }
    if (Symbol == TEXT("Legs"))
    {
        Stroke({{6, 3}, {18, 3}, {20, 21}, {14, 21}, {12, 11}, {10, 21}, {4, 21}, {6, 3}});
        Stroke({{6, 7}, {18, 7}});
    }
    if (Symbol == TEXT("Boots"))
    {
        Stroke({{5, 3}, {13, 3}, {13, 14}, {20, 17}, {21, 21}, {3, 21}, {3, 16}, {5, 13}, {5, 3}});
        Stroke({{7, 7}, {12, 7}});
        Stroke({{7, 10}, {12, 10}});
        Stroke({{7, 13}, {12, 13}});
        Stroke({{3, 18}, {20, 18}});
    }
    if (Symbol == TEXT("Backpack"))
    {
        Stroke({{9, 5}, {9, 2}, {15, 2}, {15, 5}});
        Stroke({{7, 5}, {17, 5}, {20, 9}, {20, 21}, {4, 21}, {4, 9}, {7, 5}});
        Stroke({{7, 13}, {17, 13}, {17, 18}, {7, 18}, {7, 13}});
        Stroke({{7, 9}, {17, 9}});
    }
    if (Symbol == TEXT("Belt"))
    {
        Stroke({{3, 8}, {21, 8}, {21, 16}, {3, 16}, {3, 8}});
        Stroke({{9, 7}, {15, 7}, {15, 17}, {9, 17}, {9, 7}});
        Stroke({{11, 12}, {16, 12}});
    }
    if (Symbol == TEXT("Attachments"))
    {
        Stroke({{3, 8}, {21, 8}});
        Stroke({{5, 5}, {5, 11}, {2, 11}, {2, 20}, {9, 20}, {9, 11}, {5, 11}});
        Stroke({{18, 5}, {18, 11}, {14, 11}, {14, 20}, {22, 20}, {22, 11}, {18, 11}});
    }
    if (Symbol == TEXT("Full"))
    {
        Stroke({{9, 3}, {15, 3}, {15, 7}, {9, 7}, {9, 3}});
        Stroke({{3, 10}, {8, 9}, {16, 9}, {21, 10}});
        Stroke({{8, 9}, {9, 15}, {7, 22}});
        Stroke({{16, 9}, {15, 15}, {17, 22}});
    }
    if (Symbol == TEXT("Patch"))
    {
        Stroke({{4, 3}, {20, 3}, {20, 14}, {17, 18}, {12, 22}, {7, 18}, {4, 14}, {4, 3}});
        Stroke({{12, 7}, {12, 16}});
        Stroke({{8, 11}, {16, 11}});
    }
    if (Symbol == TEXT("Surface"))
    {
        Stroke({{4, 6}, {12, 2}, {20, 6}, {20, 18}, {12, 22}, {4, 18}, {4, 6}});
        Stroke({{7, 15}, {15, 7}});
        Stroke({{9, 19}, {18, 10}});
    }
    if (Symbol == TEXT("Back"))
    {
        Stroke({{10, 5}, {3, 12}, {10, 19}});
        Stroke({{3, 12}, {21, 12}});
    }
    if (Symbol == TEXT("Apply"))
    {
        Stroke({{4, 12}, {10, 18}, {21, 6}});
    }
    if (Symbol == TEXT("RotateLeft") || Symbol == TEXT("RotateRight"))
    {
        const double Direction = Symbol == TEXT("RotateLeft") ? 1.0 : -1.0;
        const auto Mirror = [&](double X, double Y) { return FVector2D(12.0 + (X - 12.0) * Direction, Y); };
        Stroke({Mirror(4, 9), Mirror(7, 5), Mirror(15, 4), Mirror(20, 8), Mirror(21, 15), Mirror(17, 20), Mirror(9, 21)});
        Stroke({Mirror(3, 3), Mirror(4, 9), Mirror(10, 8)});
    }
    if (Symbol == TEXT("Empty"))
    {
        Stroke({{7, 3}, {17, 3}, {21, 7}, {21, 17}, {17, 21}, {7, 21}, {3, 17}, {3, 7}, {7, 3}});
        Stroke({{6, 18}, {18, 6}});
    }
    if (Symbol == TEXT("Item"))
    {
        Stroke({{3, 7}, {12, 3}, {21, 7}, {21, 18}, {12, 22}, {3, 18}, {3, 7}});
        Stroke({{3, 7}, {12, 11}, {21, 7}});
        Stroke({{12, 11}, {12, 22}});
        Stroke({{7, 5}, {16, 9}, {16, 13}});
    }
    if (Symbol == TEXT("Warning"))
    {
        Stroke({{12, 3}, {22, 21}, {2, 21}, {12, 3}});
        Stroke({{12, 9}, {12, 14}});
        Stroke({{12, 17}, {12, 18}});
    }
    return Layer;
}
