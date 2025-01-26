#pragma once
#include <QAbstractSpinBox>
#include <cmath>

class CoordinateEdit
{
    public:
        enum class Hemisphere {
            negative,
            positive,
        };

        explicit CoordinateEdit(double min, double max): m_min(min), m_max(max), m_value(qBound(min, 0.0, max)) {}

        virtual ~CoordinateEdit() {}

        double value() const
        {
            return m_value;
        }

        void setValue(const double& deg)
        {
            auto value = qBound(m_min, deg, m_max);
            if(qFuzzyCompare(value, 0.) || (value > 0. && m_value <= 0.) || (value < 0. && m_value >= 0.)) {
                Hemisphere hs = (value < 0.) ? Hemisphere::negative : ((value > 0.) ? Hemisphere::positive : m_zerroHemisphere);
                hemisphereChanged(hs);
            }
            m_value = value;
            valueChanged();
        }

        void addDegree(double deg)
        {
            setValue(m_value + deg);
        }

        void addMinute(double v)
        {
            setValue(v / 60. + m_value);
        }

        void addSecond(double v)
        {
            setValue(v / 3600. + m_value);
        }

        void setHemisphere(Hemisphere hs)
        {
            m_zerroHemisphere = hs;
            if(hs ==  Hemisphere::positive && m_value < 0.0) {
                setValue(fabs(m_value));
            }
            if(hs ==  Hemisphere::negative && m_value > 0.0) {
                setValue(-m_value);
            }
            if(qFuzzyCompare(m_value, 0.)) {
                hemisphereChanged(hs);
            }
        }

        void setMaximum(double max)
        {
            m_max = max;
            setValue(m_value);
        }

        double maximum() const
        {
            return m_max;
        }

        void setMinimum(double min)
        {
            m_min = min;
            setValue(m_value);
        }

        double minimum() const
        {
            return m_min;
        }

    protected:
        virtual void valueChanged() = 0;
        virtual void hemisphereChanged(Hemisphere hs) = 0;

    private:
        Hemisphere m_zerroHemisphere {Hemisphere::positive};
        double m_min, m_max, m_value {0};
};

class AbstractCoordinateEdit : public QAbstractSpinBox, public CoordinateEdit
{
        Q_OBJECT
    public:
        explicit AbstractCoordinateEdit(double min, double max, QWidget* parent = nullptr);

    signals:
        void valueChanged(double);

    protected:
        enum class Section {
            hemisphere,
            degree,
            minute,
            sub_minute,
            second,
            sub_second,
        };

        void stepBy(int steps) override;
        StepEnabled stepEnabled() const override;
        void keyPressEvent(QKeyEvent* event) override;
        void valueChanged()override;
        virtual bool parse(double& result) const = 0;
        virtual Section section(int pos) const = 0;
        virtual QString assambly() const = 0;
};

class LatitudeEdit : public AbstractCoordinateEdit
{
        Q_OBJECT
    public:
        explicit LatitudeEdit(QWidget* parent = nullptr);

    protected:
        void hemisphereChanged(Hemisphere hs) override;
        bool parse(double& result) const override;
        Section section(int pos) const override;
        QString assambly() const override;
};

class LongitudeEdit : public AbstractCoordinateEdit
{
        Q_OBJECT
    public:
        explicit LongitudeEdit(QWidget* parent = nullptr);

    protected:
        void hemisphereChanged(Hemisphere hs) override;
        bool parse(double& result) const override;
        Section section(int pos) const override;
        QString assambly() const override;
};

class  DeltaLatitudeEdit : public AbstractCoordinateEdit
{
        Q_OBJECT
    public:
        explicit DeltaLatitudeEdit(QWidget* parent = nullptr);

    protected:
        void hemisphereChanged(Hemisphere hs) override;
        bool parse(double& result) const override;
        Section section(int pos) const override;
        QString assambly() const override;
};

class DeltaLongitudeEdit : public AbstractCoordinateEdit
{
        Q_OBJECT
    public:
        explicit DeltaLongitudeEdit(QWidget* parent = nullptr);

    protected:
        void hemisphereChanged(Hemisphere hs) override;
        bool parse(double& result) const override;
        Section section(int pos) const override;
        QString assambly() const override;
};

class  AltitudeEdit : public AbstractCoordinateEdit
{   
        Q_OBJECT
    public:
        explicit AltitudeEdit(QWidget* parent = nullptr);

    protected:
        void hemisphereChanged(Hemisphere hs) override;
        bool parse(double& result) const override;
        Section section(int pos) const override;
        QString assambly() const override;
};

class AzimuthEdit : public AbstractCoordinateEdit
{
        Q_OBJECT
    public:
        explicit AzimuthEdit(QWidget* parent = nullptr);

    protected:
        void hemisphereChanged(Hemisphere hs) override;
        bool parse(double& result) const override;
        Section section(int pos) const override;
        QString assambly() const override;
};