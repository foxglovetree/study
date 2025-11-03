
class GridState
{
private:
    bool selected = false;

public:
    GridState()
    {
    }
    void setSelected(bool selected)
    {
        this->selected = selected;
    }
    bool getSelected()
    {
        return selected;
    }
};