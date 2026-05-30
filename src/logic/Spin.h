#pragma once

struct SpinResult
{
    bool finished = false;
    int selectedIndex = -1;
};

SpinResult updateSpin(
    AppContext& app,
    const sf::Vector2f& center,
    const sf::Vector2f& pointerTip,
    float dt)
{
    SpinResult result;

    if (!app.spin.spinning)
        return result;

    app.spin.rotation += app.spin.angularVelocity * dt;

    float effectiveDeceleration =
        app.spin.deceleration * 3600.f;

    app.spin.angularVelocity -=
        effectiveDeceleration * dt;

    if (app.spin.angularVelocity <= 0.99f)
    {
        app.spin.spinning = false;
        app.spin.angularVelocity = 0.f;

        result.finished = true;

        result.selectedIndex =
            getSelectedSegment(
                app.wheel.segments,
                center,
                pointerTip,
                app.spin.rotation
            );
    }

    return result;
}