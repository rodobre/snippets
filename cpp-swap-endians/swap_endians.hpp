template <typename T>
T swap_endians(T member)
{
    size_t sizeof_member = sizeof(member);
    size_t number_of_bits = sizeof_member * 8u;

    T new_member = 0;

    for(size_t i = 0; i < sizeof_member; i++)
    {
        new_member |= ((member >> (number_of_bits - ((i + 1u) * 8u))) & 0xFFu) << (i * 8u);
    }

    return new_member;
}
