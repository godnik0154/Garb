import React, { useState, useEffect } from "react";
import axios from "axios";
import { Link } from "react-router-dom";
import {
  DataTableContainer,
  DataTableTitle,
  DataTableLink,
  CellAction,
  ViewButton,
  DeleteButton,
  CellWithImage,
  CellImage,
  CellWithStatus,
} from "./styles/datatables";
import { DataGrid } from "@mui/x-data-grid";

const userColumns = [
  { field: "id", headerName: "ID", width: 70 },
  {
    field: "user",
    headerName: "Product",
    width: 230,
    renderCell: (params) => {
      return (
        <CellWithImage>
          <CellImage src={params.row.img} alt="avatar" />
          {params.row.name}
        </CellWithImage>
      );
    },
  },
  {
    field: "email",
    headerName: "Description",
    width: 230,
  },

  {
    field: "quantity",
    headerName: "Quantity",
    width: 100,
  },
  {
    field: "status",
    headerName: "Priority",
    width: 160,
    renderCell: (params) => {
      return (
        <CellWithStatus className={`${params.row.status}`}>
          {params.row.status}
        </CellWithStatus>
      );
    },
  },
];

//temporary data
const userRows = [
  {
    id: 1,
    name: "Product1",
    img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
    status: "active",
    email: "Description of Product 1",
    quantity: 35,
  },
  {
    id: 2,
    name: "Product2",
    img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
    email: "Description of Product 2",
    status: "passive",
    quantity: 42,
  },
  {
    id: 3,
    name: "Product3",
    img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
    email: "Description of Product 3",
    status: "pending",
    quantity: 45,
  },
  {
    id: 4,
    name: "Product4",
    img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
    email: "Description of Product 4",
    status: "active",
    quantity: 16,
  },
  {
    id: 5,
    name: "Product5",
    img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
    email: "Description of Product 5",
    status: "passive",
    quantity: 22,
  },
  {
    id: 6,
    name: "Product6",
    img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
    email: "Description of Product 6",
    status: "active",
    quantity: 15,
  },
  {
    id: 7,
    name: "Product7",
    img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
    email: "Description of Product 7",
    status: "passive",
    quantity: 44,
  },
  {
    id: 8,
    name: "Product8",
    img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
    email: "Description of Product 8",
    status: "active",
    quantity: 36,
  },
  {
    id: 9,
    name: "Product9",
    img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
    email: "Description of Product 9",
    status: "pending",
    quantity: 65,
  },
  {
    id: 10,
    name: "Product10",
    img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
    email: "Description of Product 10",
    status: "active",
    quantity: 65,
  },
];

const DataTable = () => {
  const [data, setData] = useState(userRows);
  useEffect(() => {
    let ignore = false;

    async function fetchData() {
      const result = await axios.get(
        "http://172.20.1.78:5000/api/request/view"
      );
      if (!ignore && result.status === 200) {
        setData(JSON.parse(result.data));
      }
    }

    fetchData();
    return () => {
      ignore = true;
    };
  });
  const handleDelete = (id) => {
    setData(data.filter((item) => item.id !== id));
  };

  const actionColumn = [
    {
      field: "action",
      headerName: "Action",
      width: 200,
      renderCell: (params) => {
        return (
          <CellAction>
            <Link
              to={"/items/" + params.row.id}
              style={{ textDecoration: "none" }}
            >
              <ViewButton>View</ViewButton>
            </Link>
            <DeleteButton onClick={() => handleDelete(params.row.id)}>
              Delete
            </DeleteButton>
          </CellAction>
        );
      },
    },
  ];
  return (
    <DataTableContainer>
      <DataTableTitle>
        Items
        <DataTableLink to="/items/new">Add New</DataTableLink>
      </DataTableTitle>
      <DataGrid
        rows={data}
        columns={userColumns.concat(actionColumn)}
        pageSize={9}
        rowsPerPageOptions={[9]}
        checkboxSelection
      />
    </DataTableContainer>
  );
};

export default DataTable;
